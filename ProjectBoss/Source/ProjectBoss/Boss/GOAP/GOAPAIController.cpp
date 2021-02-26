// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"

#include "../BossCharacter.h"
#include "../../ProjectBoss.h"
#include "../../ProjectBossCharacter.h"
#include "PBGOAPAction.h"
#include "../../Statistics/CombatStats.h"
#include "../../Helpers/CSVFileManager.h"
#include "Kismet/KismetSystemLibrary.h"

#pragma region include AllActions
#include "Actions/Action_Follow.h"
#include "Actions/Action_MeleeAttack.h"
#include "Actions/Action_Ultimate.h"
#include "Actions/Action_AdvancedAttack.h"
#include "Actions/Action_InvisibleRelocate.h"
#include "Actions/Action_CriticalMelee.h"
#include "Actions/Action_Heal.h"
#include "Actions/Action_SeekCover.h"
#pragma endregion

AGOAPAIController::AGOAPAIController()
{
	m_printedLastFailPlan = false;

	// Create current world state
	setCurrentWorld(GetDefaultWorldState());

	// Add array of actions available to AI
	//actions.Add(UAction_Wait::StaticClass());
	actions.Add(UAction_Follow::StaticClass());
	// Offensive Actions
	actions.Add(UAction_MeleeAttack::StaticClass());
	actions.Add(UAction_AdvancedAttack::StaticClass());
	actions.Add(UAction_InvisibleRelocate::StaticClass());
	actions.Add(UAction_CriticalMelee::StaticClass());
	actions.Add(UAction_Ultimate::StaticClass());
	// Defensive Actions
	actions.Add(UAction_Heal::StaticClass());
	actions.Add(UAction_SeekCover::StaticClass());
}

void AGOAPAIController::BeginPlay()
{
	Super::BeginPlay();

	// Create desired world state on start
	TArray<FAtom> goals;
	goals.Add(CreateAtom("damage-player", true));
	//goals.Add(CreateAtom("heal", true));

	setGoal(goals);

	// Set max depth of planner
	maxDepth = 20.0f;

	// Get references to boss and player
	m_bossPawn = Cast<ABossCharacter>(GetPawn());
	m_player = Cast<AProjectBossCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AGOAPAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Save ML data from GOAP system on exit
	if (m_planSequences.Num() > 0)
	{
		SaveMLData(m_planSequences, "project-boss-ml-data.csv");
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No sequences saved, not saving any ML data"));
	}
}

void AGOAPAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// If the player is valid and is dead, stop executing GOAP
	if (m_player && m_player->GetCurrentHealth() <= 0)
	{
		return;
	}

	TArray<UGOAPAction*> lastPlan = getPlan();

	// Every tick, execute GOAP to create a plan
	bool success = executeGOAP();
	
	TArray<UGOAPAction*> currentPlan = getPlan();

	// Check for difference in plans if they contain actions
	if (lastPlan.Num() > 0 && currentPlan.Num() > 0)
	{
		// Check if different plan via different action names
		if (lastPlan.Num() == currentPlan.Num())
		{
			for (int i = 0; i < lastPlan.Num(); i++)
			{
				// If names are different, actions are too
				if (lastPlan[i]->getName() != currentPlan[i]->getName())
				{
					// Save last plan sequence
					UE_LOG(LogTemp, Log, TEXT("lastPlan and currentPlan are different! Last Action '%s' isnt equal to current '%s' action "), *lastPlan[i]->getName(), *currentPlan[i]->getName());
					m_planSequences.Add(lastPlan);
				}
			}
		}
		// else if lastPlan and currentPlan lengths are different, also changed
		else if (lastPlan.Num() != currentPlan.Num())
		{
			UE_LOG(LogTemp, Log, TEXT("lastPlan and currentPlan are different! Different lengths"))
			// Save last plan sequence
			m_planSequences.Add(lastPlan);
		}
	}
	

	if (success)
	{
		// If success, print to screen
		PrintCurrentGOAPPlan();
	}
	else
	{
		// Once plan is no longer a success... (finished executing last plan)
		
		// Save timings from previous actions when no current plan determined
		if (lastPlan.Num() > 0)
		{
			m_planSequences.Add(lastPlan);
		}

		// Update all GOAP actions with new costs from Machine Learning
		// Query python for new cost values
		//UpdateActionCostsFromML();

		// Determine new state for the GOAP system
		TArray<FAtom> nextWorldState = DetermineNextWorldState();

		if (nextWorldState.Num() <= 0)
		{
			// Only print to screen once
			if (m_printedLastFailPlan)
				return;

			// Set flag
			m_printedLastFailPlan = true;

			// Couldnt determine next world state, show error
			UE_LOG(LogGOAP, Error, TEXT("Unable to determine the next world state!"));
#if PROJ_BOSS_SCREEN_DEBUG
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::Red, TEXT("GOAP Plan: Unable to create"));
			}
#endif
		}
		else
		{
			// Reset print flag
			if (m_printedLastFailPlan)
				m_printedLastFailPlan = false;

			// nextWorldState found, set targets and allow executing plan
			SetNewWorldTargets(nextWorldState);
		}
	}
}

FAtom AGOAPAIController::CreateAtom(FString name, bool val)
{
	FAtom atm;
	atm.name = name;
	atm.value = val;
	return atm;
}

void AGOAPAIController::PrintCurrentGOAPPlan()
{
	// Gets current GOAP plan. Check if plan has actions
	TArray<UGOAPAction*> goapPlan = getPlan();

	if (goapPlan.Num() <= 0)
		return;

	// Append each action to string with '->' to separate
	FString planString = "GOAP Plan: ";
	for (int i = goapPlan.Num() - 1; i >= 0; i--)
	{
		planString += "'" + goapPlan[i]->name + "'";
		if (i >= 1)
			planString += "->";
	}

#if PROJ_BOSS_SCREEN_DEBUG
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::White, planString);
	}
#endif
}

void AGOAPAIController::SetNewWorldTargets(TArray<FAtom> targets)
{
	// Reset all targets to initial state
	setCurrentWorld(GetDefaultWorldState());

	// Set new goal of reset world
	setGoal(targets);
}

TArray<FAtom> AGOAPAIController::DetermineNextWorldState()
{
	TArray<FAtom> targets;

	ABossCharacter* boss = Cast<ABossCharacter>(GetPawn());
	// if boss health is less than 35%, use self-heal
	if (WorldContainsAtom("heal", false) && boss->CanHeal())
	{
		targets.Add(CreateAtom("heal", true));
		return targets;
	}
	
	// Make boss endlessly aim to damage player if current world state isn't that already
	/*TArray<FAtom> world = getCurrentWorldStateAtoms();
	for (int i = 0; i < world.Num(); i++) {
		if (world[i].name == "damage-player" && world[i].value)
		{
			
		}
	}*/

	// If player is valid and has enough health
	targets.Add(CreateAtom("damage-player", true));

	return targets;
}

bool AGOAPAIController::WorldContainsAtom(FString atomName, bool state)
{
	for (FAtom atom : currentWorld)
	{
		if (atom.name == atomName && atom.value == state)
		{
			return true;
		}
	}
	return false;
}

TArray<FAtom> AGOAPAIController::GetDefaultWorldState()
{
	// Set default world state
	TArray<FAtom> state;
	state.Add(CreateAtom("in-melee-range", false));
	state.Add(CreateAtom("damage-player", false));
	state.Add(CreateAtom("heal", false));
	state.Add(CreateAtom("in-cover", false));
	state.Add(CreateAtom("is-invisible", false));

	return state;
}

void AGOAPAIController::SaveMLData(TArray<TArray<UGOAPAction*>> allPlanSequences, FString fileName)
{
	// Check if all plans actually contain any
	if (allPlanSequences.Num() <= 0)
	{
		UE_LOG(LogGOAP, Error, TEXT("All Plans are empty!"));
		return;
	}

	TArray<FMLData> mlData;
	int totalActions = 0;

	// Iterate over all plan sequences
	for (TArray<UGOAPAction*> sequence : allPlanSequences)
	{
		// Does this sequence contains any actions?
		if (sequence.Num() <= 0)
		{
			UE_LOG(LogGOAP, Error, TEXT("Sequence contains no actions!"));
			continue;
		}

		// Iterate through all actions in current sequence...
		for (UGOAPAction* goapAction : sequence)
		{
			UPBGOAPAction* pbAction = Cast<UPBGOAPAction>(goapAction);
			if (pbAction)
			{
				FMLData actionData;
				// Store the action name, helps for debug
				actionData.ActionName = pbAction->getName();

				// Get the base cost of the action
				actionData.BaseCost = pbAction->GetBaseCost();

				// Get the elapsed seconds on the action
				actionData.AverageExecuteSeconds = pbAction->GetAverageExecuteTime();

				// Get ability damage
				actionData.Damage = pbAction->GetDamage();
				
				// gets ability attempts and successful attempts
				actionData.Attempts = 0;
				actionData.SuccessfulAttempts = 0;
				int abilityIndex = pbAction->GetAbilityIndex();
				UCombatStats* stats = m_bossPawn->GetCombatStats();
				// Check data is valid
				if (stats != nullptr && abilityIndex > -1)
				{
					actionData.Attempts = stats->GetAbilityAttempts(abilityIndex);
					actionData.SuccessfulAttempts = stats->GetAbilitySuccessfulAttempts(abilityIndex);
				}

				// Set final ML data cost
				actionData.FinalCost = pbAction->getCost();

				// Add data to list
				mlData.Add(actionData);
			}

			totalActions++;
		}
	}

	// Save ML data to csv file
	FString baseDir = UKismetSystemLibrary::GetProjectDirectory();
	bool isSuccess = UCSVFileManager::AppendData(mlData, baseDir, fileName);
	if (isSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Saved '%d' sequences with '%d' actions to '%s%s'"), allPlanSequences.Num(), totalActions, *baseDir, *fileName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error when saving latest CSV data!"));
	}
}

void AGOAPAIController::UpdateActionCostsFromML()
{

}
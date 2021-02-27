// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"

#include "Kismet/KismetSystemLibrary.h"

#include "../BossCharacter.h"
#include "../../ProjectBoss.h"
#include "../../ProjectBossCharacter.h"
#include "PBGOAPAction.h"
#include "../../Statistics/CombatStats.h"
#include "../../Helpers/CSVFileManager.h"
#include "../../ML/UpdateCosts_PyActor.h"
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

	// Get references to boss and player
	m_bossPawn = Cast<ABossCharacter>(GetPawn());
	m_player = Cast<AProjectBossCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	// Get reference to Python bridge actor
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AUpdateCosts_PyActor::StaticClass());
	m_pythonActor = Cast<AUpdateCosts_PyActor>(actor);


	// Create desired world state on start
	TArray<FAtom> goals;
	goals.Add(CreateAtom("damage-player", true));
	// Set initial goal
	setGoal(goals);

	// Set max depth of planner
	maxDepth = 20.0f;
}

void AGOAPAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	TArray<UGOAPAction*> bossActions = GetAuxActions();

	// Save ML data on all actions
	if (bossActions.Num() > 0)
	{
		SaveMLData(bossActions, "project-boss-ml-data.csv");
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

	if (success)
	{
		// If success, print to screen
		PrintCurrentGOAPPlan();
	}
	else
	{
		// Once plan is no longer a success... (finished executing last plan)
		
		// Update all GOAP actions with new costs from Machine Learning
		// Query python for new cost values
		UpdateActionCostsFromML();

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

void AGOAPAIController::SaveMLData(TArray<UGOAPAction*> goapActions, FString fileName)
{
	// Check if all plans actually contain any
	if (goapActions.Num() <= 0)
	{
		UE_LOG(LogGOAP, Error, TEXT("All GOAP actions are empty!"));
		return;
	}

	TArray<FMLData> mlData;

	// Iterate over all plan sequences
	// Iterate through all actions in current sequence...
	for (UGOAPAction* goapAction : goapActions)
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

			// Set ability attempts and successful attempts to default values
			actionData.DidSucceed = pbAction->GetDidSucceed();

			// Set final ML data cost
			actionData.FinalCost = pbAction->getCost();

			// Add data to list
			mlData.Add(actionData);
		}
	}

	// Save ML data to csv file
	FString baseDir = UKismetSystemLibrary::GetProjectDirectory();
	bool isSuccess = UCSVFileManager::AppendData(mlData, baseDir, fileName);
	if (isSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Saved '%d' actions to CSV file '%s%s'"), goapActions.Num(), *baseDir, *fileName);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error when saving latest CSV data!"));
	}
}

void AGOAPAIController::UpdateActionCostsFromML()
{
	// Get all goap actions and iterate
	TArray<UGOAPAction*> allGoapActions = GetAuxActions();
	for (UGOAPAction* action : allGoapActions)
	{
		// Cast to PB GOAP action
		UPBGOAPAction* pbAction = Cast<UPBGOAPAction>(action);
		// Call Python Actor and python code to genereate new cost from ML'd data
		float mlCost = m_pythonActor->GenerateCost(pbAction->GetBaseCost(), pbAction->GetDidSucceed(), pbAction->GetDamage(), pbAction->GetAverageExecuteTime());
		// Set new cost of the action
		pbAction->UpdateCost(mlCost);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"
#include "../BossCharacter.h"
#pragma region AllActionsInclude
#include "Actions/Action_Wait.h"
#include "Actions/Action_Follow.h"
#include "Actions/Action_MeleeAttack.h"
#include "Actions/Action_Ultimate.h"
#include "Actions/Action_AdvancedAttack.h"
#include "Actions/Action_AbilityOne.h"
#pragma endregion

AGOAPAIController::AGOAPAIController()
{
	m_isPlanning = false;

	// Create current world state
	currentWorld.Add(CreateAtom("in-melee-range", false));
	//currentWorld.Add(CreateAtom("in-medium-range", false));
	currentWorld.Add(CreateAtom("damage-player", false));
	//currentWorld.Add(CreateAtom("is-player-alive", true));
	//currentWorld.Add(CreateAtom("knife-fury-available", true));

	// Add array of actions available to AI
	//actions.Add(UAction_Wait::StaticClass());
	actions.Add(UAction_Follow::StaticClass());
	actions.Add(UAction_MeleeAttack::StaticClass());
	actions.Add(UAction_AdvancedAttack::StaticClass());
	actions.Add(UAction_AbilityOne::StaticClass());
	actions.Add(UAction_Ultimate::StaticClass());
}

void AGOAPAIController::BeginPlay()
{
	Super::BeginPlay();

	// Create desired world state
	TArray<FAtom> goals;
	goals.Add(CreateAtom("damage-player", true));

	setGoal(goals);

	// Set max depth of planner
	maxDepth = 20.0f;
}

void AGOAPAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	if (m_isPlanning)
		return;

	m_isPlanning = true;
	// Every tick, execute GOAP to create a plan
	bool success = executeGOAP();
	
	TArray<UGOAPAction*> lastPlan = getPlan();

	if (success)
	{
		PrintCurrentGOAPPlan();
	}
	else
	{
		TArray<FAtom> nextWorldState = DetermineNextWorldState();
		if (nextWorldState.Num() <= 0)
		{
			// Couldnt determine next world state, show error
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::Red, TEXT("GOAP Plan: Unable to create"));
			}
		}
		else
		{
			SetNewWorldTargets(nextWorldState);
		}
	}

	m_isPlanning = false;
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

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::White, planString);
	}
}

void AGOAPAIController::SetNewWorldTargets(TArray<FAtom> targets)
{
	// Reset all targets to initial state
	TArray<FAtom> goals;
	goals.Add(CreateAtom("damage-player", false));
	goals.Add(CreateAtom("in-melee-range", false));
	setCurrentWorld(goals);

	// Set new goal of reset world
	setGoal(targets);
}

TArray<FAtom> AGOAPAIController::DetermineNextWorldState()
{
	TArray<FAtom> targets;

	ABossCharacter* boss = Cast<ABossCharacter>(GetPawn());
	// if boss health is less than 35%, use self-heal
	if (boss->GetCurrentHealth() <= boss->GetTotalHealth() * 0.35)
	{
		targets.Add(CreateAtom("heal", true));
		return targets;
	}
	else
	{
		// Make boss endlessly aim to damage player if current world state isn't that already
		TArray<FAtom> world = getCurrentWorldStateAtoms();
		for (int i = 0; i < world.Num(); i++) {
			if (world[i].name == "damage-player" && world[i].value)
			{
				targets.Add(CreateAtom("damage-player", true));
			}
		}
	}

	return targets;
}

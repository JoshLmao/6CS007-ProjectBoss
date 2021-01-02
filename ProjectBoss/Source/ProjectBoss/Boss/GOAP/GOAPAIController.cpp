// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"
#pragma region AllActionsInclude
#include "Actions/Action_Wait.h"
#include "Actions/Action_Follow.h"
#include "Actions/Action_MeleeAttack.h"
#include "Actions/Action_KnifeFury.h"
#pragma endregion

AGOAPAIController::AGOAPAIController()
{
	// Create current world state
	currentWorld.Add(CreateAtom("in-melee-range", false));
	currentWorld.Add(CreateAtom("in-medium-range", false));
	currentWorld.Add(CreateAtom("damage-player", false));
	currentWorld.Add(CreateAtom("is-player-alive", true));
	currentWorld.Add(CreateAtom("knife-fury-available", true));

	// Add array of actions available to AI
	actions.Add(UAction_Wait::StaticClass());
	actions.Add(UAction_Follow::StaticClass());
	actions.Add(UAction_MeleeAttack::StaticClass());
	actions.Add(UAction_KnifeFury::StaticClass());
}

void AGOAPAIController::BeginPlay()
{
	Super::BeginPlay();

	// Create desired world state
	TArray<FAtom> goals;
	goals.Add(CreateAtom("damage-player", true));

	setGoal(goals);
}

void AGOAPAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Every tick, execute GOAP to create a plan
	bool success = executeGOAP();
	
	if (success)
	{
		PrintCurrentGOAPPlan();
	}
	else
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::Red, TEXT("Unable to create GOAP Plan!"));
		//UE_LOG(LogTemp, Log, TEXT("Unable to create GOAP execute plan."));
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
	TArray<UGOAPAction*> goapPlan = getPlan();

	if (goapPlan.Num() <= 0)
		return;

	FString planString = "Plan: ";
	for (int i = goapPlan.Num() - 1; i >= 0; i--)
	{
		planString += goapPlan[i]->name;
		if (i >= 1)
			planString += "->";
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::White, planString);
	}
}
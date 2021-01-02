// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"
#include "Actions/Action_Follow.h"
#include "Actions/Action_MeleeAttack.h"

AGOAPAIController::AGOAPAIController()
{
	// Create current world state
	currentWorld.Add(CreateAtom("in-range", false));
	currentWorld.Add(CreateAtom("damage-player", false));

	// Add array of actions available to AI
	actions.Add(UAction_Follow::StaticClass());
	actions.Add(UAction_MeleeAttack::StaticClass());
}

void AGOAPAIController::BeginPlay()
{
	Super::BeginPlay();

	// Create desired world state
	FAtom goal = CreateAtom("damage-player", true);
	TArray<FAtom> goals;
	goals.Add(goal);

	setGoal(goals);
}

void AGOAPAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Every tick, execute GOAP
	bool success = executeGOAP();
	TArray<UGOAPAction*> currentPlan = getPlan();
	if (success)
	{
		if (currentPlan.Num() > 0)
		{
			PrintCurrentPlan(currentPlan);
		}
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

void AGOAPAIController::PrintCurrentPlan(TArray<UGOAPAction*> goapPlan)
{
	FString planString = "Plan: ";
	for (int i = 0; i < goapPlan.Num(); i++)
	{
		planString += goapPlan[i]->name;
		if (i < goapPlan.Num() - 1)
			planString += "->";
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(0, 15.0f, FColor::White, planString);
	}
}
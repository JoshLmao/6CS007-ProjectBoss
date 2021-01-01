// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"
#include "Actions/Action_Follow.h"

AGOAPAIController::AGOAPAIController()
{
	// Create current world state
	FAtom followPlayer;
	followPlayer.name = "follow-player";
	followPlayer.value = false;
	currentWorld.Add(followPlayer);

	// Create target world state
	FAtom followPlayerTarget;
	followPlayerTarget.name = "follow-player";
	followPlayerTarget.value = true;
	desiredWorld.Add(followPlayerTarget);

	// Add array of actions available to AI
	actions.Add(UAction_Follow::StaticClass());
}

void AGOAPAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGOAPAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Every tick, execute GOAP
	bool success = executeGOAP();
	if (!success)
	{
		UE_LOG(LogTemp, Log, TEXT("Unable to create GOAP execute plan."));
	}
}

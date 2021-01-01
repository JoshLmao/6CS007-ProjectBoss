// Fill out your copyright notice in the Description page of Project Settings.


#include "GOAPAIController.h"
#include "Actions/Action_Follow.h"

AGOAPAIController::AGOAPAIController()
{
	FAtom followPlayer;
	followPlayer.name = "follow-player";
	followPlayer.value = false;
	currentWorld.Add(followPlayer);

	FAtom followPlayerTarget;
	followPlayerTarget.name = "follow-player";
	followPlayerTarget.value = true;
	desiredWorld.Add(followPlayerTarget);

	actions.Add(UAction_Follow::StaticClass());
}

void AGOAPAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGOAPAIController::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	bool success = executeGOAP();
	if (success)
	{
		UE_LOG(LogTemp, Log, TEXT("Successful!"));
	}
}

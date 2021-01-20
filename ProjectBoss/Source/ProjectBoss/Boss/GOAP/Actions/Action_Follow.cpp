// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Follow.h"
#include "../../../ProjectBossCharacter.h"
#include "GameFramework/Controller.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../BossCharacter.h"

UAction_Follow::UAction_Follow()
{
	// Initialize action with name, cost and target class
	name = "follow target";
	cost = 0.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	// Create preconditions for action
	preconditions.Add(CreateAtom("in-melee-range", false));

	// Create effects to world state once action is complete
	effects.Add(CreateAtom("in-melee-range", true));
}

bool UAction_Follow::checkProceduralPrecondition(APawn* actingPawn)
{
	Super::checkProceduralPrecondition(actingPawn);

	bool set = TrySetTarget(actingPawn);

	return set;
}

bool UAction_Follow::doAction(APawn* aiCharPawn)
{
	Super::doAction(aiCharPawn);

	// Check if action has valid target
	AActor* targetActor = getTarget();
	if (!targetActor)
	{
		// No target has been set, action will fail
		UE_LOG(LogTemp, Log, TEXT("No target set!"));
		return false;
	}

	ABossCharacter* boss = Cast<ABossCharacter>(aiCharPawn);
	// Check boss isn't stunned, cant move if so
	if (boss->GetIsStunned() == true)
	{
		boss->CancelChaseTarget();
		return false;
	}

	// Move AI to player
	bool reachedTarget = boss->ChaseTarget(targetActor);
	if (reachedTarget)
	{
		//UE_LOG(LogTemp, Log, TEXT("Arrived at player's position!"));
		return true;
	}

	// Return false if no AI Controller
	return false;
}

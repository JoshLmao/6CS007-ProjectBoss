// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Follow.h"
#include "../../../ProjectBossCharacter.h"
#include "GameFramework/Controller.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

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

	// Get AI controller for movement
	AAIController* controller = Cast<AAIController>(aiCharPawn->GetController());
	if (controller)
	{
		// Rotate AI base model towards player
		FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(aiCharPawn->GetActorLocation(), targetActor->GetActorLocation());
		FRotator finalRot = aiCharPawn->GetActorRotation();
		finalRot.Yaw = lookAtRotation.Yaw + -90.0f;
		aiCharPawn->SetActorRotation(finalRot);
		
		// Set AIController Control Rotation (head look rotation) to be at player
		finalRot.Yaw = 180 + finalRot.Yaw;
		aiCharPawn->GetController()->SetControlRotation(finalRot);
		
		// Move AI to player
		EPathFollowingRequestResult::Type type = controller->MoveToActor(targetActor, FollowRadius);

		if (type == EPathFollowingRequestResult::Type::AlreadyAtGoal)
		{
			//UE_LOG(LogTemp, Log, TEXT("Arrived at player's position!"));
			return true;
		}
	}

	// Return false if no AI Controller
	return false;
}

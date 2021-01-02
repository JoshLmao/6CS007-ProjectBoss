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
	name = "follow player";
	cost = 0.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom preRange;
	preRange.name = "in-melee-range";
	preRange.value = false;
	preconditions.Add(preRange);

	// Create effects to world state once action is complete
	FAtom followEffect;
	followEffect.name = "in-melee-range";
	followEffect.value = true;
	effects.Add(followEffect);
}

bool UAction_Follow::checkProceduralPrecondition(APawn* actingPawn)
{
	Super::checkProceduralPrecondition(actingPawn);

	// Find the closest actor to the pawn
	TArray<AActor*> actorTargets = getTargetsList(actingPawn);
	AActor* closest = nullptr;
	float closestDist = TNumericLimits<float>::Max();
	for (int i = 0;  i < actorTargets.Num(); i++)
	{
		float dist = FVector::Dist(actorTargets[i]->GetActorLocation(), actingPawn->GetActorLocation());
		if (dist < closestDist)
		{
			closest = actorTargets[i];
			closestDist = dist;
		}
	}

	// Set the target pawn to the closest one
	setTarget(closest);

	return true;
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
			UE_LOG(LogTemp, Log, TEXT("Arrived at player's position!"));
			return true;
		}
	}

	// Return false if no AI Controller
	return false;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_MeleeAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"

UAction_MeleeAttack::UAction_MeleeAttack()
{
	name = "attack melee";
	cost = 30.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	// precons of action
	preconditions.Add(CreateAtom("in-melee-range", true));
	preconditions.Add(CreateAtom("damage-player", false));

	// Effects of action
	effects.Add(CreateAtom("damage-player", true));
	//effects.Add(CreateAtom("in-melee-range", false));
}

bool UAction_MeleeAttack::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

	bool set = TrySetTarget(pawn);

	return set;
}

bool UAction_MeleeAttack::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	AActor* targetActor = getTarget();
	if (!IsValid(targetActor))
	{
		return false;
	}

	// Check target is within melee range
	float meleeRange = 150.0f;
	if (FVector::Dist(targetActor->GetActorLocation(), pawn->GetActorLocation()) > meleeRange)
	{
		// finished action. target too far away
		return true;
	}

	ABossCharacter* bossChar = Cast<ABossCharacter>(pawn);
	if (bossChar)
	{
		// Perform melee attack.
		bossChar->PerformMeleeAttack();
	}

	// Never complete this action, dont return true
	return false;
}

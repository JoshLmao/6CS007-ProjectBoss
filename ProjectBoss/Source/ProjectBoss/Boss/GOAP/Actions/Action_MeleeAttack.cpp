// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_MeleeAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"

UAction_MeleeAttack::UAction_MeleeAttack()
{
	name = "attack melee";
	BaseCost = 10.0f;
	cost = BaseCost;
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

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);

	if (boss)
	{
		// Constantly look at actor, regardless of in range
		boss->LookAtActor(targetActor->GetActorLocation());

		// Check target is within melee range
		// Multiply 2 since we are using radius
		float meleeDistRange = boss->GetMeleeRadius() * 2;
		float dist = FVector::Dist(targetActor->GetActorLocation(), pawn->GetActorLocation());
		if (dist > meleeDistRange)
		{
			// finished action. target too far away
			return true;
		}

		// Perform melee attack.
		boss->PerformMeleeAttack();

		//UpdateCost(BaseCost);
	}

	// Never complete this action, dont return true
	return false;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_InvisibleRelocate.h"
#include "../../BossCharacter.h"
#include "../../../ProjectBossCharacter.h"
#include "NavigationSystem.h"

UAction_InvisibleRelocate::UAction_InvisibleRelocate()
{
	m_rndLoc = FVector::ZeroVector;
	m_movedToRndLoc = false;
	
	name = "invisible relocate";
	BaseCost = cost = 10.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	effects.Add(CreateAtom("is-invisible", true));
}

bool UAction_InvisibleRelocate::checkProceduralPrecondition(APawn* p)
{
	Super::checkProceduralPrecondition(p);

	bool setTarget = TrySetTarget(p);

	// Check ability isnt on cooldown
	ABossCharacter* boss = Cast<ABossCharacter>(p);
	if (boss->GetAbilityOneCooldown() > 0)
	{
		return false;
	}

	if (boss && boss->IsPerformingAbility())
	{
		return false;
	}

	return setTarget;
}

bool UAction_InvisibleRelocate::doAction(APawn* p)
{
	Super::doAction(p);

	ABossCharacter* boss = Cast<ABossCharacter>(p);

	// Check if stunned
	bool isStunned = boss->GetIsStunned();
	if (isStunned)
	{
		// Cancel move to location and wait until unstunned
		boss->CancelMoveToLocation();

		// Reveal from invisible while stunned
		if (boss->GetIsInvisible())
		{
			boss->SetInvisible(false);
		}

		// False to continue to execute action
		return false;
	}
	else
	{
		// Set to invisible if not already
		if (!boss->GetIsInvisible())
		{
			boss->SetInvisible(true);
		}

		// Move to relocate location first
		if (!m_movedToRndLoc)
		{
			// Find random location around player to relocate and attack from if not been set before
			if (m_rndLoc == FVector::ZeroVector)
			{
				m_rndLoc = UNavigationSystemV1::GetRandomReachablePointInRadius(p->GetWorld(), getTarget()->GetActorLocation(), 750.0f);
			}

			// Move towards relocate location and set to true once reached
			bool isInRange = boss->MoveToLocation(m_rndLoc);
			if (isInRange) {
				m_movedToRndLoc = true;
			}
		}
		else
		{
			// Move towards actor
			AActor* targetActor = getTarget();
			bool isInRange = boss->MoveToLocation(targetActor->GetActorLocation());
			if (isInRange) {
				return true;
			}
		}
	}
	
	return false;
}
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

	BossAbilityIndex = EAbilities::One;
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

	// False if another ability is in progress
	if (boss && boss->IsPerformingAbility())
	{
		return false;
	}

	if (setTarget)
	{
		AActor* player = getTarget();
		AProjectBossCharacter* pbPlayer = Cast<AProjectBossCharacter>(player);
		// Check pawn is 100 units away from player before performing
		if (FVector::Distance(pbPlayer->GetActorLocation(), p->GetActorLocation()) < 100.0f)
		{
			return false;
		}
	}

	return setTarget;
}

bool UAction_InvisibleRelocate::doAction(APawn* p)
{
	Super::doAction(p);

	SetActionInProgress(true);

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
			// Find random location around self to relocate and attack from if not been set before
			if (m_rndLoc == FVector::ZeroVector)
			{
				m_rndLoc = UNavigationSystemV1::GetRandomReachablePointInRadius(p->GetWorld(), p->GetActorLocation(), 750.0f);
			}

			// Move towards relocate location and set to true once reached
			bool isInRange = boss->MoveToLocation(m_rndLoc);
			if (isInRange) 
			{
				m_movedToRndLoc = true;
			}
		}
		else
		{
			// Move towards actor
			AActor* targetActor = getTarget();
			bool isInRange = boss->MoveToLocation(targetActor->GetActorLocation());
			if (isInRange) 
			{
				SetActionInProgress(false);
				return true;
			}
		}
	}
	
	return false;
}

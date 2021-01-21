// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_AbilityOne.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"

UAction_AbilityOne::UAction_AbilityOne()
{
	name = "ability one";
	cost = 20.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	effects.Add(CreateAtom("damage-player", true));
}

bool UAction_AbilityOne::checkProceduralPrecondition(APawn* p)
{
	Super::checkProceduralPrecondition(p);

	bool setTarget = TrySetTarget(p);

	// Check ability isnt on cooldown
	ABossCharacter* boss = Cast<ABossCharacter>(p);
	if (boss->GetAbilityOneCooldown() > 0)
	{
		return false;
	}

	// Unable to perform action if another ability is being performed
	if (boss->IsPerformingAbility())
	{
		return false;
	}
	
	return setTarget;
}

bool UAction_AbilityOne::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	if (IsValid(m_boss))
	{
		bool completedAbility = UpdateFSM();
		if (completedAbility)
		{
			// Once ability finished, reset boss reference
			m_boss = nullptr;
		}
		return completedAbility;
	}
	else
	{
		m_boss = Cast<ABossCharacter>(pawn);
		SetState(EState::Enter);
	}
	return false;
}

bool UAction_AbilityOne::UpdateFSM()
{
	switch (m_currentState)
	{
		case EState::Enter:
			BeginStart();
			break;
		case EState::Invisible:
			UpdateInvisible();
			break;
		case EState::Exit:
			BeginExit();
			return true;
			break;
	}
	return false;
}

void UAction_AbilityOne::SetState(EState targetState)
{
	m_currentState = targetState;
}

void UAction_AbilityOne::BeginStart()
{
	// Run once, set invisible textures and move to chase player
	if (m_boss)
	{
		m_boss->SetInvisible(true);
		SetState(EState::Invisible);
	}
}

void UAction_AbilityOne::UpdateInvisible()
{
	if (m_boss)
	{
		bool isStunned = m_boss->GetIsStunned();
		if (isStunned)
		{
			m_boss->CancelMoveToLocation();
			return;
		}
		else
		{
			AActor* targetActor = getTarget();
			bool isInRange = m_boss->MoveToLocation(targetActor->GetActorLocation());
			if (isInRange) {
				SetState(EState::Exit);
			}
		}
	}
}

void UAction_AbilityOne::BeginExit()
{
	if (m_boss)
	{
		m_boss->SetInvisible(false);
		m_boss->PerformMeleeAttack();

		m_boss->BeginAbilityOneCooldown();
	}
}

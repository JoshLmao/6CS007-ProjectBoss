// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Heal.h"
#include "../../BossCharacter.h"
#include "../../../ProjectBossCharacter.h"

UAction_Heal::UAction_Heal()
{
	name = "heal";
	BaseCost = cost = 5.0f;

	// Preconditions
	preconditions.Add(CreateAtom("in-cover", true));

	// Effects
	effects.Add(CreateAtom("heal", true));
	effects.Add(CreateAtom("in-cover", false));
}

bool UAction_Heal::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	bool setTarget = TrySetTarget(pawn);
	
	if (boss)
	{
		if (boss->GetHealCooldown() > 0)
		{
			return false;
		}

		if (!boss->CanHeal())
		{
			return false;
		}
	}

	return setTarget;
}

bool UAction_Heal::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);

	if (boss)
	{
		boss->PerformHeal();

		// Exit action once completed healing
		bool isHealing = boss->IsPerformingAbility(4);
		if (!isHealing)
		{
			return true;
		}
	}

	return false;
}


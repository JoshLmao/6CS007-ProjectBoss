// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Heal.h"
#include "../../BossCharacter.h"
#include "../../../ProjectBossCharacter.h"
#include "../../../UtilityHelper.h"

UAction_Heal::UAction_Heal()
{
	name = "heal";
	BaseCost =  5.0f;
	cost = 0.0f;

	// Preconditions
	preconditions.Add(CreateAtom("in-cover", true));

	// Effects
	effects.Add(CreateAtom("heal", true));
	effects.Add(CreateAtom("in-cover", false));

	BossAbilityIndex = EAbilities::Heal;
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

	if (setTarget)
	{
		AProjectBossCharacter* player = Cast<AProjectBossCharacter>(getTarget());
		// Health Difference will be positive if boss has less health than Player, negative if boss has more health than Player
		float healthDiff = UtilityHelper::GetHealthDifference(player->GetCurrentHealth(), player->GetTotalHealth(), boss->GetCurrentHealth(), boss->GetTotalHealth());
		float smallIncrements = healthDiff / 100;
		UpdateCost(BaseCost + smallIncrements);
	}

	return setTarget;
}

bool UAction_Heal::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	SetActionInProgress(true);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);

	if (boss)
	{
		boss->PerformHeal();

		// Exit action once completed healing
		bool isHealing = boss->IsPerformingAbility(4);
		if (!isHealing)
		{
			SetActionInProgress(false);
			return true;
		}
	}

	return false;
}


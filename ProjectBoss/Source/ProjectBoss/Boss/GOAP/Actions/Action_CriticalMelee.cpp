// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_CriticalMelee.h"
#include "../../BossCharacter.h"
#include "../../../ProjectBossCharacter.h"

UAction_CriticalMelee::UAction_CriticalMelee()
{
	name = "critical melee";
	cost = 1.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	preconditions.Add(CreateAtom("is-invisible", true));

	effects.Add(CreateAtom("is-invisible", false));
	effects.Add(CreateAtom("damage-player", true));
}

bool UAction_CriticalMelee::checkProceduralPrecondition(APawn* p)
{
	Super::checkProceduralPrecondition(p);

	bool setTarget = TrySetTarget(p);

	return setTarget;
}

bool UAction_CriticalMelee::doAction(APawn* p)
{
	Super::doAction(p);
	
	ABossCharacter* boss = Cast<ABossCharacter>(p);

	// Set crit multiplier and perform melee
	boss->SetMeleeCritMultiplier(boss->GetAbilityOneCritMultiplier());
	boss->PerformMeleeAttack();

	// Remove invis from boss
	if (boss->GetIsInvisible())
	{
		boss->SetInvisible(false);
	}

	// Begin cooldown
	boss->BeginAbilityOneCooldown();

	return true;
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_CriticalMelee.h"
#include "../../BossCharacter.h"
#include "../../../ProjectBossCharacter.h"
#include "../../../ProjectBoss.h"

UAction_CriticalMelee::UAction_CriticalMelee()
{
	m_eventBound = false;

	name = "critical melee";
	BaseCost = cost = 10.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	preconditions.Add(CreateAtom("is-invisible", true));

	effects.Add(CreateAtom("is-invisible", false));
	effects.Add(CreateAtom("damage-player", true));

	BossAbilityIndex = EAbilities::Melee;
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
	
	SetActionInProgress(true);

	ABossCharacter* boss = Cast<ABossCharacter>(p);
	
	if (!m_eventBound)
	{
		boss->OnMeleeSucceeded.AddDynamic(this, &UAction_CriticalMelee::MeleeSuccess);
		m_eventBound = true;
	}

	// Set crit multiplier and perform melee
	float multiplier = boss->GetAbilityOneCritMultiplier();
	boss->SetMeleeCritMultiplier(multiplier);
	boss->PerformMeleeAttack();

	// Set action damage
	Damage = boss->GetMeleeDamage() * (multiplier / 100);

	// Remove invis from boss
	if (boss->GetIsInvisible())
	{
		boss->SetInvisible(false);
	}

	// Begin cooldown
	boss->BeginAbilityOneCooldown();

	SetActionInProgress(false);

	return true;
}

void UAction_CriticalMelee::MeleeSuccess()
{
	// If not in progress, set last as success
	if (!GetIsInProgress())
	{
		SetLastPerformanceDidSucceed(true);
	}

	// Set succeed flag to true
	SetDidSucceed(true);
}
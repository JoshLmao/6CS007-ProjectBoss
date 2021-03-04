// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Ultimate.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../../../UtilityHelper.h"

UAction_Ultimate::UAction_Ultimate()
{
	name = "ability ultimate";
	cost = BaseCost = 20.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom dmgEffect;
	dmgEffect.name = "damage-player";
	dmgEffect.value = true;
	effects.Add(dmgEffect);

	BossAbilityIndex = EAbilities::Ultimate;
}

bool UAction_Ultimate::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

	// Get action self and target
	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	bool setTarget = TrySetTarget(pawn);

#ifdef GOAP_COSTS_TO_HEALTH_DIFF
	if (setTarget)
	{
		AProjectBossCharacter* player = Cast<AProjectBossCharacter>(getTarget());

		// Health Difference will be positive if boss has less health than Player, negative if boss has more health than Player
		float healthDiff = UtilityHelper::GetHealthDifference(player->GetCurrentHealth(), player->GetTotalHealth(), boss->GetCurrentHealth(), boss->GetTotalHealth());
		float smallIncrements = healthDiff / 100;
		UpdateCost(BaseCost + smallIncrements);
	}
#endif

	// Check ability action isn't on cooldown
	if (boss->GetUltimateCooldown() > 0)
	{
		OnPreconditionExit(false);
		return false;
	}

	if (boss->IsPerformingAbility(EAbilities::Melee) || boss->IsPerformingAbility(EAbilities::One) || boss->IsPerformingAbility(EAbilities::Advanced))
	{
		OnPreconditionExit(false);
		return false;
	}

	// Don't perform action if too close to target
	float dist = FVector::Distance(boss->GetActorLocation(), getTarget()->GetActorLocation());
	if (dist <= 750.0f)
	{
		OnPreconditionExit(false);
		return false;
	}

	// Check we have target and ultimate isn't on cooldown
	return setTarget;
}

bool UAction_Ultimate::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	SetActionInProgress(true);
	if (boss)
	{
		AActor* targetActor = getTarget();
		boss->PerformUltimate(targetActor);

		// Update dmg if not set
		float actionDmg = boss->GetUltimateDamage();
		if (Damage != actionDmg)
		{
			Damage = actionDmg;
		}

		bool isPerforming = boss->IsPerformingAbility(EAbilities::Ultimate);
		if (!isPerforming)
		{
			UE_LOG(LogTemp, Log, TEXT("Completed Ultimate action!"));
			SetActionInProgress(false);
			return true;
		}
	}

	return false;
}

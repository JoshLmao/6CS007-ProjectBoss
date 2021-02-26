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

	if (setTarget)
	{
		AProjectBossCharacter* player = Cast<AProjectBossCharacter>(getTarget());

		// Health Difference will be positive if boss has less health than Player, negative if boss has more health than Player
		float healthDiff = UtilityHelper::GetHealthDifference(player->GetCurrentHealth(), player->GetTotalHealth(), boss->GetCurrentHealth(), boss->GetTotalHealth());
		float smallIncrements = healthDiff / 100;
		UpdateCost(BaseCost + smallIncrements);
	}

	// Check ability action isn't on cooldown
	if (boss->GetUltimateCooldown() > 0)
	{
		return false;
	}

	// Don't perform action if too close to target
	float dist = FVector::Distance(boss->GetActorLocation(), getTarget()->GetActorLocation());
	if (dist <= 750.0f)
	{
		return false;
	}

	// Check we have target and ultimate isn't on cooldown
	return setTarget;
}

bool UAction_Ultimate::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	if (boss)
	{
		AActor* targetActor = getTarget();
		boss->PerformUltimate(targetActor);

		bool isPerforming = boss->IsPerformingAbility(EAbilities::Ultimate);
		if (!isPerforming)
		{
			//UE_LOG(LogGOAPAction, Log, TEXT("Completed Ultimate action!"));
			Damage = boss->GetUltimateDamage();
			return true;
		}
	}

	return false;
}

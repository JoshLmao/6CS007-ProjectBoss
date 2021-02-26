// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_AdvancedAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../../../UtilityHelper.h"

UAction_AdvancedAttack::UAction_AdvancedAttack()
{
	name = "ability advanced";
	BaseCost = cost = 20.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	effects.Add(CreateAtom("damage-player", true));

	BossAbilityIndex = EAbilities::Advanced;
}

bool UAction_AdvancedAttack::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

	bool setTarget = TrySetTarget(pawn);

	// Make sure ability isnt on cooldown
	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	if (boss->GetAdvancedAbilityCooldown() > 0)
	{
		return false;
	}

	// Unable to perform action if another ability is being performed
	if (boss->IsPerformingAbility(0) || boss->IsPerformingAbility(2) || boss->IsPerformingAbility(3))
	{
		return false;
	}

	float minDist = 400.0f;
	if (FVector::Distance(boss->GetActorLocation(), getTarget()->GetActorLocation()) < minDist)
	{
		return false;
	}

	if (setTarget)
	{
		AProjectBossCharacter* player = Cast<AProjectBossCharacter>(getTarget());
		// Health Difference will be positive if boss has less health than Player, negative if boss has more health than Player
		float healthDiff = UtilityHelper::GetHealthDifference(player->GetCurrentHealth(), player->GetTotalHealth(), boss->GetCurrentHealth(), boss->GetTotalHealth());
		float smallIncrements = healthDiff / 100;
		UpdateCost(BaseCost + smallIncrements);
	}

	// Check we have target and ultimate isn't on cooldown
	return setTarget;
}

bool UAction_AdvancedAttack::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	
	SetActionInProgress(true);

	if (boss)
	{
		AActor* targetActor = getTarget();
		boss->PerformAdvancedAttack(targetActor);

		bool isPerforming = boss->IsPerformingAbility(1);
		if (!isPerforming)
		{
			// Only return true (action finished) once ability ended
			Damage = boss->GetAdvancedAbilityDamage();
			SetActionInProgress(false);
			return true;
		}
	}

	return false;
}

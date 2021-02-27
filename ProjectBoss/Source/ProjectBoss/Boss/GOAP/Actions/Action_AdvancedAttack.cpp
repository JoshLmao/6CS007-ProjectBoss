// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_AdvancedAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "../../../UtilityHelper.h"
#include "../../../ProjectBoss.h"

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
	
	if (setTarget)
	{
		AProjectBossCharacter* player = Cast<AProjectBossCharacter>(getTarget());
		// Health Difference will be positive if boss has less health than Player, negative if boss has more health than Player
		float healthDiff = UtilityHelper::GetHealthDifference(player->GetCurrentHealth(), player->GetTotalHealth(), boss->GetCurrentHealth(), boss->GetTotalHealth());
		float smallIncrements = healthDiff / 100;
		UpdateCost(BaseCost + smallIncrements);
	}

	if (boss->GetAdvancedAbilityCooldown() > 0)
	{
		OnPreconditionExit(false);
		return false;
	}

	// Unable to perform action if another ability is being performed
	if (boss->IsPerformingAbility(EAbilities::Melee) || boss->IsPerformingAbility(EAbilities::One) || boss->IsPerformingAbility(EAbilities::Ultimate))
	{
		OnPreconditionExit(false);
		return false;
	}

	float minDist = 400.0f;
	if (FVector::Distance(boss->GetActorLocation(), getTarget()->GetActorLocation()) < minDist)
	{
		OnPreconditionExit(false);
		return false;
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

		// Update dmg if not set
		float actionDmg = boss->GetAdvancedAbilityDamage();
		if (Damage != actionDmg)
		{
			Damage = actionDmg;
		}

		bool isPerforming = boss->IsPerformingAbility(EAbilities::Advanced);
		if (!isPerforming)
		{
			// Only return true (action finished) once ability ended
			
			SetActionInProgress(false);
			return true;
		}
	}

	return false;
}

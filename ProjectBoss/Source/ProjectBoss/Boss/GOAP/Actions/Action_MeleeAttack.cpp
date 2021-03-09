// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_MeleeAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"
#include "../../../UtilityHelper.h"

UAction_MeleeAttack::UAction_MeleeAttack()
{
	m_eventBound = false;

	name = "attack melee";
	BaseCost = cost = 50.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	// precons of action
	preconditions.Add(CreateAtom("in-melee-range", true));
	preconditions.Add(CreateAtom("damage-player", false));

	// Effects of action
	effects.Add(CreateAtom("damage-player", true));
	//effects.Add(CreateAtom("in-melee-range", false));
}

bool UAction_MeleeAttack::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

	bool set = TrySetTarget(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);

	if (set)
	{
		AProjectBossCharacter* player = Cast<AProjectBossCharacter>(getTarget());
		if (player && player->GetCurrentHealth() <= 0)
		{
			return false;
		}

#ifdef GOAP_COSTS_TO_HEALTH_DIFF
		// Health Difference will be positive if boss has less health than Player, negative if boss has more health than Player
		float healthDiff = UtilityHelper::GetHealthDifference(player->GetCurrentHealth(), player->GetTotalHealth(), boss->GetCurrentHealth(), boss->GetTotalHealth());
		float smallIncrements = healthDiff / 100;
		UpdateCost(BaseCost + smallIncrements);
#endif
	}

	if (IsValid(boss) && boss->IsPerformingAbility())
	{
		return false;
	}

	return set;
}

bool UAction_MeleeAttack::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	// Check target has been set
	AActor* targetActor = getTarget();
	if (!IsValid(targetActor))
	{
		return false;
	}

	// Cast to BossCharacter
	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	AProjectBossCharacter* player = Cast<AProjectBossCharacter>(targetActor);

	if (!m_eventBound)
	{
		boss->OnMeleeSucceeded.AddDynamic(this, &UAction_MeleeAttack::MeleeSuccess);
		m_eventBound = true;
	}

	Damage = boss->GetMeleeDamage();
	SetActionInProgress(true);

	if (boss)
	{
		// Constantly look at actor, regardless of in range
		boss->LookAtActor(targetActor->GetActorLocation());

		// Check target is within melee range
		// Multiply 2 since we are using radius
		float meleeDistRange = boss->GetMeleeRadius() * 2;
		float dist = FVector::Dist(targetActor->GetActorLocation(), pawn->GetActorLocation());
		if (dist > meleeDistRange)
		{
			// finished action. target too far away
			SetActionInProgress(false);
			return true;
		}

		// Get health difference and set attack rate difficulty so boss attacks slower if player has less health
		float healthDiff = UtilityHelper::GetHealthDifference(player->GetCurrentHealth(), player->GetTotalHealth(), boss->GetCurrentHealth(), boss->GetTotalHealth());
		float normalized = healthDiff / 1000;
		//UE_LOG(LogTemp, Log, TEXT("Value: %f"), normalized);
		boss->SetAttackRateDifficulty(normalized);
			
		// Perform melee attack.
		boss->PerformMeleeAttack();

		//UpdateCost(BaseCost);
	}

	// Never complete this action, dont return true
	return false;
}


void UAction_MeleeAttack::MeleeSuccess()
{
	// If not in progress, set last as success
	if (!GetIsInProgress())
	{
		SetLastPerformanceDidSucceed(true);
	}

	// Set succeed flag to true
	SetDidSucceed(true);

}
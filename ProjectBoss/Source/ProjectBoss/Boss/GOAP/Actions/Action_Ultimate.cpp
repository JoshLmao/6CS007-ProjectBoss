// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Ultimate.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"
#include "Kismet/GameplayStatics.h"

UAction_Ultimate::UAction_Ultimate()
{
	name = "ability ultimate";
	cost = BaseCost = 20.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom dmgEffect;
	dmgEffect.name = "damage-player";
	dmgEffect.value = true;
	effects.Add(dmgEffect);
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

		// Positive if boss has less health than Player, Negative if boss has more health than Player
		float playerHealthLost = player->GetTotalHealth() - player->GetCurrentHealth();
		float bossHealthLost = boss->GetTotalHealth() - boss->GetCurrentHealth();
		float healthDiff = (playerHealthLost - bossHealthLost) / 100;
		
		UpdateCost(BaseCost + healthDiff);
	}

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
		return true;
	}

	return false;
}

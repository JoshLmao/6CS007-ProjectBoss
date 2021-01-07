// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_AdvancedAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"
#include "Kismet/GameplayStatics.h"

UAction_AdvancedAttack::UAction_AdvancedAttack()
{
	name = "rmb attack";
	cost = 0.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom dmgEffect;
	dmgEffect.name = "damage-player";
	dmgEffect.value = true;
	effects.Add(dmgEffect);
}

bool UAction_AdvancedAttack::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);

	TArray<AActor*> targets = getTargetsList(pawn);
	if (targets.Num() <= 0)
	{
		return false;
	}

	for (int i = 0; i < targets.Num(); i++)
	{
		// Set target to Player
		AProjectBossCharacter* bossChar = Cast<AProjectBossCharacter>(targets[i]);
		if (bossChar)
		{
			setTarget(targets[i]);
		}
	}

	// Check we have target and ultimate isn't on cooldown
	return getTarget() != nullptr;
}

bool UAction_AdvancedAttack::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	if (boss)
	{
		AActor* targetActor = getTarget();
		boss->PerformAdvancedAttack(targetActor);
		return true;
	}

	return false;
}

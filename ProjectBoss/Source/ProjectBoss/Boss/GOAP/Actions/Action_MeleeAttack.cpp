// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_MeleeAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"

UAction_MeleeAttack::UAction_MeleeAttack()
{
	name = "Melee Attack";
	cost = 00.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom range;
	range.name = "in-melee-range";
	range.value = true;
	preconditions.Add(range);
	FAtom preDmg;
	preDmg.name = "damage-player";
	preDmg.value = false;
	preconditions.Add(preDmg);

	FAtom dmgPlayer;
	dmgPlayer.name = "damage-player";
	dmgPlayer.value = true;
	effects.Add(dmgPlayer);
	FAtom inRange;
	inRange.name = "in-melee-range";
	inRange.value = false;
	effects.Add(inRange);
}

bool UAction_MeleeAttack::checkProceduralPrecondition(APawn* pawn)
{
	Super::checkProceduralPrecondition(pawn);

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
			//UE_LOG(LogTemp, Log, TEXT("Set Target of MeleeAttack!"));
		}
	}

	return true;
}

bool UAction_MeleeAttack::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	AActor* targetActor = getTarget();
	if (!targetActor)
	{
		return false;
	}

	ABossCharacter* bossChar = Cast<ABossCharacter>(pawn);
	if (bossChar)
	{
		bossChar->PerformMeleeAttack();
		//return true; // completed action
	}

	return false;
}

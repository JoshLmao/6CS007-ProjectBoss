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
	range.name = "in-range";
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
}

bool UAction_MeleeAttack::checkProceduralPrecondition(APawn* p)
{
	Super::checkProceduralPrecondition(p);

	if (getTarget())
		return true;

	TArray<AActor*> targets = getTargetsList(p);
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
			UE_LOG(LogTemp, Log, TEXT("Set Target of MeleeAttack!"));
			return true;
		}
	}

	return false;
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
		/*float dist = FVector::Dist(bossChar->GetActorLocation(), targetActor->GetActorLocation());
		if (dist > 50.0f)
		{
			UE_LOG(LogTemp, Log, TEXT("Out of range to melee attack player!"));
			return false;
		}*/

		bossChar->PerformMeleeAttack();
		return true;
	}

	return false;
}

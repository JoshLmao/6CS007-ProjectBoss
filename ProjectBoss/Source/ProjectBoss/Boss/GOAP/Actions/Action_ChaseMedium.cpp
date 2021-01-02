// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_ChaseMedium.h"
#include "../../../ProjectBossCharacter.h"

UAction_ChaseMedium::UAction_ChaseMedium()
{
	name = "chase medium";
	cost = 0.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom preRange;
	preRange.name = "in-medium-range";
	preRange.value = false;
	preconditions.Add(preRange);

	FAtom dmgPlayer;
	dmgPlayer.name = "in-medium-range";
	dmgPlayer.value = true;
	effects.Add(dmgPlayer);
}

bool UAction_ChaseMedium::checkProceduralPrecondition(APawn* pawn)
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
			setTarget(targets[i]);
	}

	return true;

}

bool UAction_ChaseMedium::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	AActor* targetActor = getTarget();
	if (!targetActor)
	{
		return false;
	}

	return false;
}


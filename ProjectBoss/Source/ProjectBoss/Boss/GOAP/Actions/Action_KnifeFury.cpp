// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_KnifeFury.h"
#include "../../../ProjectBossCharacter.h"

UAction_KnifeFury::UAction_KnifeFury()
{
	name = "knife fury";
	cost = 150.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom preCooldown;
	preCooldown.name = "knife-fury-available";
	preCooldown.value = true;
	preconditions.Add(preCooldown);
	FAtom preInRange;
	preInRange.name = "in-melee-range";
	preInRange.value = true;
	preconditions.Add(preInRange);

	FAtom effectCooldown;
	effectCooldown.name = "knife-fury-available";
	effectCooldown.value = false;
	effects.Add(effectCooldown);
	FAtom eInRange;
	eInRange.name = "in-melee-range";
	eInRange.value = false;
	effects.Add(eInRange);
}

bool UAction_KnifeFury::checkProceduralPrecondition(APawn* pawn)
{
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
			return true;
		}
	}

	return false;
}

bool UAction_KnifeFury::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	// to do

	return true;
}

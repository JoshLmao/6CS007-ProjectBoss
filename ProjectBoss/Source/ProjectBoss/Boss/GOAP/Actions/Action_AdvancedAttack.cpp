// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_AdvancedAttack.h"
#include "../../../ProjectBossCharacter.h"
#include "../../BossCharacter.h"
#include "Kismet/GameplayStatics.h"

UAction_AdvancedAttack::UAction_AdvancedAttack()
{
	name = "ability advanced";
	cost = 20.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	effects.Add(CreateAtom("damage-player", true));
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

	// Check we have target and ultimate isn't on cooldown
	return setTarget;
}

bool UAction_AdvancedAttack::doAction(APawn* pawn)
{
	Super::doAction(pawn);

	ABossCharacter* boss = Cast<ABossCharacter>(pawn);
	
	if (boss)
	{
		AActor* targetActor = getTarget();
		boss->PerformAdvancedAttack(targetActor);

		bool isPerforming = boss->IsPerformingAbility(1);
		if (!isPerforming)
		{
			// Only return true (action finished) once ability ended
			return true;
		}
	}

	return false;
}

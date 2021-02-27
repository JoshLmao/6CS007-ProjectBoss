// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_MeleeAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_MeleeAttack : public UPBGOAPAction
{
	GENERATED_BODY()

public:
	UAction_MeleeAttack();

	// Override precondition checking for GOAPAction
	bool checkProceduralPrecondition(APawn* p) override;

	// Override doAction from GOAPAction to perform melee attack
	bool doAction(APawn* pawn) override;

private:
	bool m_eventBound;

	UFUNCTION()
	void MeleeSuccess();
};

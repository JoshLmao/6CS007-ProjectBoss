// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_AdvancedAttack.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_AdvancedAttack : public UPBGOAPAction
{
	GENERATED_BODY()

public:
	UAction_AdvancedAttack();

	bool checkProceduralPrecondition(APawn* p) override;
	bool doAction(APawn* pawn) override;
};

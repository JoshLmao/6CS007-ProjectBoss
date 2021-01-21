// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_Follow.generated.h"

/**
 * GOAP action to move towards player
 */
UCLASS()
class PROJECTBOSS_API UAction_Follow : public UPBGOAPAction
{
	GENERATED_BODY()

public:
	UAction_Follow();

	bool doAction(APawn* pawn) override;

	bool checkProceduralPrecondition(APawn* pawn) override;
};

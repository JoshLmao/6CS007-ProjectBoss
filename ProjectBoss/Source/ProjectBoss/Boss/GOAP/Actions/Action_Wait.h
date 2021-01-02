// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "Action_Wait.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_Wait : public UGOAPAction
{
	GENERATED_BODY()

public:
	UAction_Wait();

	// Override precondition checking for GOAPAction
	bool checkProceduralPrecondition(APawn* p) override;

	// Override doAction from GOAPAction to perform melee attack
	bool doAction(APawn* pawn) override;

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "Action_ChaseMedium.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_ChaseMedium : public UGOAPAction
{
	GENERATED_BODY()

public:
	UAction_ChaseMedium();
		
	bool doAction(APawn* pawn) override;

	bool checkProceduralPrecondition(APawn* p) override;
};

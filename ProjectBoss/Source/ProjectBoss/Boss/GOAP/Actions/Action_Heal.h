// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_Heal.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_Heal : public UPBGOAPAction
{
	GENERATED_BODY()
	
public:
	UAction_Heal();

	bool checkProceduralPrecondition(APawn* p) override;

	bool doAction(APawn* pawn) override;
};

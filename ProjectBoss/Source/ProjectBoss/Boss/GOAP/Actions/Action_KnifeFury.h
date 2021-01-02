// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "Action_KnifeFury.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_KnifeFury : public UGOAPAction
{
	GENERATED_BODY()
	
public:
	UAction_KnifeFury();

	bool doAction(APawn* pawn) override;

	bool checkProceduralPrecondition(APawn* p) override;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_SeekCover.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_SeekCover : public UPBGOAPAction
{
	GENERATED_BODY()
	
public:
	UAction_SeekCover();

	bool checkProceduralPrecondition(APawn* p) override;

	bool doAction(APawn* pawn) override;

	FVector rndLoc;
};
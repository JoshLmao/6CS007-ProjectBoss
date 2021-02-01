// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_InvisibleRelocate.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_InvisibleRelocate : public UPBGOAPAction
{
	GENERATED_BODY()

public:
	UAction_InvisibleRelocate();

	bool checkProceduralPrecondition(APawn* p) override;
	bool doAction(APawn* p) override;

private:
	FVector m_rndLoc;
	bool m_movedToRndLoc;
};

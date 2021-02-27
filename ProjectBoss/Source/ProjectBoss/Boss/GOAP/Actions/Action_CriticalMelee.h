// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_CriticalMelee.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_CriticalMelee : public UPBGOAPAction
{
	GENERATED_BODY()

public:
	UAction_CriticalMelee();

	bool checkProceduralPrecondition(APawn* p) override;
	bool doAction(APawn* p) override;

private:
	bool m_eventBound;

	UFUNCTION()
	void MeleeSuccess();
};

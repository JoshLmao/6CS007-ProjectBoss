// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPController.h"
#include "GOAPAIController.generated.h"

/**
 * Overall GOAP AI controller that sets goals to get a plan for the boss to execute
 */
UCLASS()
class PROJECTBOSS_API AGOAPAIController : public AGOAPController
{
	GENERATED_BODY()

public:
	AGOAPAIController();

	virtual void BeginPlay() override;

protected:
	virtual void Tick(float deltaTime) override;

private:
	FAtom CreateAtom(FString name, bool val);

	void PrintCurrentGOAPPlan();
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPController.h"
#include "GOAPAIController.generated.h"

/**
 * 
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
};

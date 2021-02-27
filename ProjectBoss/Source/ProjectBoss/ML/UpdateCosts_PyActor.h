// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PyActor.h"
#include "UpdateCosts_PyActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API AUpdateCosts_PyActor : public APyActor
{
	GENERATED_BODY()
	
public:
	AUpdateCosts_PyActor();

	//// Updates the GOAP controller's specific action to a new cost value
	//UFUNCTION()
	//void UpdateActionCost(int action, float newCost);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	class ABossCharacter* m_boss;

	FTimerHandle m_updateTimerHandle;
	
	/*
	*	METHODS
	*/
public:
	// Generates a new cost for a GOAP Action from Machine Learning from it's inputs.
	UFUNCTION()
	float GenerateCost(float baseCost, bool wasSuccess, float damage, float averageSeconds);

	UFUNCTION()
	void CallPython();
};

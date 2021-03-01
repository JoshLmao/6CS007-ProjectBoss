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

	/*
	*	VARIABLES
	*/
public:
	// Multiplier to multiply the ML prediction by
	UPROPERTY(EditAnywhere)
	float PredictionMultiplier;

private:
	// Reference to boss character
	class ABossCharacter* m_boss;
	
	// Python method name to initialize machine learning
	const FString INIT_ML = "init_ml";
	// Python method name for predicting a new cost. 
	// Method returns a float of the cost
	const FString PREDICT_COST = "predict_cost";
	// Pythong method name for setting a prediction multiplier
	const FString SET_MULTIPLIER = "set_predict_multiplier";

	/*
	*	METHODS
	*/
public:
	// Generates a new cost for a GOAP Action from Machine Learning from it's inputs.
	UFUNCTION()
	float GenerateCost(FString actionName, float baseCost, bool wasSuccess, float damage, float averageSeconds);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	UFUNCTION()
	void CallPythonExample();

};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "PBGOAPAction.generated.h"

/**
 * Base class for all Project Boss GOAP actions
 */
UCLASS()
class PROJECTBOSS_API UPBGOAPAction : public UGOAPAction
{
	GENERATED_BODY()

public:
	UPBGOAPAction();

	/*
	*	VARIABLES
	*/
protected:
	// Base cost of the action. A constant value that never changes
	float BaseCost;
	// Amount of dmg dealt on this action
	float Damage;
	// Ability Index for the Boss, if applicable
	int BossAbilityIndex;

private:
	// Current time elapsed to execute this action
	float m_timeToExecute;
	// Is the action in progress
	bool m_isInProgress;
	// List of all time to executes for the plan
	TArray<float> m_allTimeToExecutes;

	/*
	*	METHODS
	*/
public:
	// Returns the avergae time to execute this action
	float GetAverageExecuteTime();

	// Gets the amount of damage the action deals
	virtual float GetDamage();
	// Gets the ability index, if action is an ability. -1 is isn't an ability
	int GetAbilityIndex();
	// Gets the base cost of this action
	float GetBaseCost();

protected:
	// Override doAction called every frame to execute the action
	virtual bool doAction(APawn* p) override;

	// Creates an Atom with the given name and value
	FAtom CreateAtom(FString atmName, bool value);
	// Tries to get the targetsType actor in the scene and sets it to the target
	bool TrySetTarget(APawn* pawn);
	// Update action's cost
	void UpdateCost(float newCost);

	// Sets if the action is in progress or not. 
	// If in progress, the elapsed time will increment until stopped.
	void SetActionInProgress(bool inProgress);
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "PBGOAPAction.generated.h"

USTRUCT()
struct FActionPerformance
{
	GENERATED_BODY()

	// Was the action able to succeed (Last damage, hit target, perform it's duty)
	bool DidSucceed;
	// Time in seconds to execute the action
	float TimeToExecute;

	// Default constructor
	FActionPerformance() { };

	// Constructor to set all variables
	FActionPerformance(bool didSucceed, float timeToExecute)
	{
		DidSucceed = didSucceed;
		TimeToExecute = timeToExecute;
	}
};

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
	UPROPERTY()
	float m_timeToExecute;
	// Is the action in progress
	UPROPERTY()
	bool m_isInProgress;
	// Did the action succeed in it's aim?
	UPROPERTY()
	bool m_didSucceed;
	// List of history of performance for this action
	TArray<FActionPerformance> m_performanceHistory;
	// Is this class instance bound to listen to AbilitySuccess event
	bool m_eventBound;

	/*
	*	METHODS
	*/
public:
	// Returns the average time to execute this action
	float GetAverageExecuteTime();
	// Gets the success rate of this action as a percentage (0-100)
	float GetSuccessRateAsPercentage();

	// Gets the amount of damage the action deals
	virtual float GetDamage();
	// Gets the ability index, if action is an ability. -1 is isn't an ability
	int GetAbilityIndex();
	// Gets the base cost of this action
	float GetBaseCost();
	// Gets if the action succeed when it last executed
	bool GetDidSucceed();

	// Update action's cost. Adds the additionalCost value with the base cost
	void UpdateCost(float additionalCost);

protected:
	// Override doAction called every frame to execute the action
	virtual bool doAction(APawn* p) override;
	virtual bool checkProceduralPrecondition(APawn* pawn) override;

	// Creates an Atom with the given name and value
	FAtom CreateAtom(FString atmName, bool value);
	// Tries to get the targetsType actor in the scene and sets it to the target
	bool TrySetTarget(APawn* pawn);

	// Sets if the action is in progress or not. 
	// If in progress, the elapsed time will increment until stopped.
	void SetActionInProgress(bool inProgress);
	// Set if the action succeeded or not
	void SetDidSucceed(bool didSucceed);
	// Get if the action still in progress or not
	bool GetIsInProgress();
	// Sets the last performance statistic DidSucceed property
	void SetLastPerformanceDidSucceed(bool didSucceed);
	// Call in precondition if exited false to check data is saved correctly
	void OnPreconditionExit(bool state);

private:
	UFUNCTION()
	void OnAbilitySucceeded(int abilityIndex);

};

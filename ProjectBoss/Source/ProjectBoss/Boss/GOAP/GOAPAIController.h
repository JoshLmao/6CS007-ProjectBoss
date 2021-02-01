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
	bool m_printedLastFailPlan;
	class ABossCharacter* m_bossPawn;
	
	// Creates and returns an Atom with the name and value
	FAtom CreateAtom(FString name, bool val);

	// Prints the current GOAP plan
	void PrintCurrentGOAPPlan();
	
	// Set a target world state for the GOAP system to achieve
	void SetNewWorldTargets(TArray<FAtom> targets);

	// Determines what the boss character should aim to do next and returns the world state
	TArray<FAtom> DetermineNextWorldState();

	// Checks if the world has a current atom in state
	bool WorldContainsAtom(FString atomName, bool state);

	// Check if the world state has reached it's target world state. Returns true if so
	bool HasWorldReachedGoal();

	// Gets the default world state
	TArray<FAtom> GetDefaultWorldState();

};

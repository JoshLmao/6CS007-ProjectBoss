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

	/*
	*	VARIABLES
	*/
private:
	// Have we printed last failed plan to screen?
	bool m_printedLastFailPlan;
	// Reference to boss pawn
	class ABossCharacter* m_bossPawn;
	// Reference to player character
	class AProjectBossCharacter* m_player;
	// Array of all sequences executed in this play session
	TArray<TArray<class UGOAPAction*>> m_planSequences;
	// Reference to Python bridge actor used for updating action costs
	class AUpdateCosts_PyActor* m_pythonActor;
	
	// Should this session's action sequences be saved to a csv file?
	bool m_saveMLData;

	/*
	*	METHODS
	*/
	virtual void BeginPlay() override;

protected:
	virtual void Tick(float deltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
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

	// Gets the default world state
	TArray<FAtom> GetDefaultWorldState();

	// Saves data needed for ML to file name (Uses project base directory for file path)
	// Requires a TArray of all sequences (another TArray of all UGOAPAction's)
	void SaveMLData(TArray<TArray<class UGOAPAction*>> allPlanSequences, FString fileDirectory, FString fileName);
	// Updates all GOAP actions with a new cost
	void UpdateActionCostsFromML();
};

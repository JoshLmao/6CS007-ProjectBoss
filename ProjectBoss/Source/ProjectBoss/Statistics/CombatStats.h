// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CombatStats.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UCombatStats : public UObject
{
	GENERATED_BODY()
	
public:
	UCombatStats();

private:
	int m_successfullAttacks;
	int m_totalAttacks;
	
	float m_totalDealtDamage;

	// Dictionary of ability index and attempts to perform the ability
	UPROPERTY()
	TMap<int, int> m_abilityAttempt;
	// Dictionary of ability index and success count
	UPROPERTY()
	TMap<int, int> m_abilitySuccess;

	/*
	*	METHODS
	*/
public:
	// Adds an attack to the count
	void AddAttack();
	// Adds a successful attack 
	void AddSuccessfulAttack();
	
	// Adds to the attemp
	void AddAbilityAttempt(int abilityIndex);
	// Adds to the successful ability count
	void AddAbilitySuccess(int abilityIndex);
	// Gets the amount of attempts of this ability
	int GetAbilityAttempts(int abilityIndex);
	// Gets the amount of successful attempts of this ability
	int GetAbilitySuccessfulAttempts(int abilityIndex);

	// Gets the total number of attacks performed
	float GetTotalAttacks();
	// Gets the total number of successful attacks performed
	float GetSuccessfulAttacks();
	// Gets the total damage dealt
	float GetTotalDamageDealt();

	// Builds a string of all of the current stats
	FString GetAllStatsString();
};

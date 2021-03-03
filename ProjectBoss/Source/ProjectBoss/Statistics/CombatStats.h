// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CombatStats.generated.h"

USTRUCT()
struct FAbilityCount
{
	GENERATED_BODY()
	// Ability index
	int Ability;
	// Count for the ability
	int Count;

	// Default counstrctor
	FAbilityCount() { }

	// Constructor for variables
	FAbilityCount(int abil, int count) 
	{
		Ability = abil;
		Count = count;
	}
};

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
	// Amount of attacks that landed
	int m_successfullAttacks;
	// Amount of melee attacks attemped. Includes attacks that may not have landed
	int m_totalAttacks;
	// Amount of damage dealt to another actor
	float m_totalDealtDamage;
	// Amount of damage recieved
	float m_totalDamageRecieved;
	// Amount of health recovered through healing
	float m_totalHealthHealed;

	// Dictionary of ability index and attempts to perform the ability
	UPROPERTY()
	TArray<FAbilityCount> m_abilityAttempt;
	// Dictionary of ability index and success count
	UPROPERTY()
	TArray<FAbilityCount> m_abilitySuccess;

	/*
	*	METHODS
	*/
public:
	// Adds an attack to the count
	void AddAttack();
	// Gets the total number of attacks performed
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	float GetTotalAttacks();

	// Adds a successful attack 
	void AddSuccessfulAttack();
	// Gets the total number of successful attacks performed
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	float GetSuccessfulAttacks();

	// Adds to the attemp
	void AddAbilityAttempt(int abilityIndex);
	// Gets the amount of attempts of this ability
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	int GetAbilityAttempts(int abilityIndex);

	// Adds to the successful ability count
	void AddAbilitySuccess(int abilityIndex);
	// Gets the amount of successful attempts of this ability
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	int GetAbilitySuccessfulAttempts(int abilityIndex);

	// Adds an amount to the damage dealt to enemies
	void AddDamageDealt(float amount);
	// Gets the total damage dealt
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	float GetTotalDamageDealt();

	// Adds an amount to the damage amount recieved
	void AddDamageRecieved(float amount);
	// Gets the total damage recieved
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	float GetTotalDamageRecieved();
	
	// Adds an amount to the current health healed
	void AddHealthHealed(float amount);
	// Gets the amount of health healed
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	float GetTotalHealthHealed();

	// Builds a string of all of the current stats
	UFUNCTION(BlueprintCallable, Category = "Combat Statistics")
	FString GetAllStatsString();

private:
	// Checks if ability is 
	int ContainsAbility(TArray<FAbilityCount> arr, int abilIndex);
};

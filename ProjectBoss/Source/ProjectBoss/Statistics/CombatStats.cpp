// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatStats.h"

UCombatStats::UCombatStats()
{
}

void UCombatStats::AddAttack()
{
	m_totalAttacks++;
}

void UCombatStats::AddSuccessfulAttack()
{
	m_successfullAttacks++;
}

void UCombatStats::AddAbilityAttempt(int abilityIndex)
{
	int index = ContainsAbility(m_abilityAttempt, abilityIndex);
	if (index >= 0) 
	{
		m_abilityAttempt[index].Count += 1;
	} 
	else 
	{
		m_abilitySuccess.Add(FAbilityCount(abilityIndex, 1));
	}
}

void UCombatStats::AddAbilitySuccess(int abilityIndex)
{
	int index = ContainsAbility(m_abilitySuccess, abilityIndex);
	if (index >= 0)
	{
		m_abilitySuccess[index].Count += 1;
	}
	else 
	{
		m_abilitySuccess.Add(FAbilityCount(abilityIndex, 1));
	}
}

int UCombatStats::GetAbilityAttempts(int abilityIndex)
{
	int index = ContainsAbility(m_abilityAttempt, abilityIndex);
	if (index >= 0)
	{
		return m_abilityAttempt[index].Count;
	}
	else
	{
		return 0;
	}
}

int UCombatStats::GetAbilitySuccessfulAttempts(int abilityIndex)
{
	int index = ContainsAbility(m_abilitySuccess, abilityIndex);
	if (index >= 0)
	{
		return m_abilitySuccess[index].Count;
	}
	else 
	{
		return 0;
	}
}

float UCombatStats::GetTotalAttacks()
{
	return m_totalAttacks;
}

float UCombatStats::GetSuccessfulAttacks()
{
	return m_successfullAttacks;
}

float UCombatStats::GetTotalDamageDealt()
{
	return m_totalDealtDamage;
}

FString UCombatStats::GetAllStatsString()
{
	FString output;
	// Add total and successful attacks
	output += "Total/Successful Attacks: " + FString::SanitizeFloat(m_totalAttacks) + " / " + FString::SanitizeFloat(m_successfullAttacks);
	output += "\n";

	if (m_abilityAttempt.Num() > 0)
	{
		// Append abilities 
		for (FAbilityCount pair : m_abilityAttempt)
		{
			output += "Ability '" + FString::SanitizeFloat(pair.Ability) + "' attempted '" + FString::SanitizeFloat(pair.Count) + "' times";
			output += "\n";
		}
	}

	if (m_abilitySuccess.Num() > 0)
	{
		for (FAbilityCount pair : m_abilityAttempt)
		{
			output += "Ability '" + FString::SanitizeFloat(pair.Ability) + "' succeessfully landed '" + FString::SanitizeFloat(pair.Count) + "' times";
			output += "\n";
		}
	}

	return output;
}


int UCombatStats::ContainsAbility(TArray<FAbilityCount> arr, int abilIndex)
{
	for (int i = 0; i < arr.Num(); i++) 
	{
		FAbilityCount abilCount = arr[i];
		if (abilCount.Ability == abilIndex) 
		{
			return i;
		}
	}
	return -1;
}
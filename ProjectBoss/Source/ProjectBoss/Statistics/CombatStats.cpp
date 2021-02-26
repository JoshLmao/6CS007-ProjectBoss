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
	if (m_abilityAttempt.Contains(abilityIndex)) {
		m_abilityAttempt[abilityIndex] += 1;
	} else {
		m_abilityAttempt.Add(abilityIndex, 1);
	}
}

void UCombatStats::AddAbilitySuccess(int abilityIndex)
{
	if (m_abilitySuccess.Contains(abilityIndex)) {
		m_abilitySuccess[abilityIndex] += 1;
	}
	else {
		m_abilitySuccess.Add(abilityIndex, 1);
	}
}

int UCombatStats::GetAbilityAttempts(int abilityIndex)
{
	if (m_abilityAttempt.Contains(abilityIndex))
	{
		return m_abilityAttempt[abilityIndex];
	}
	else
	{
		return 0;
	}
}

int UCombatStats::GetAbilitySuccessfulAttempts(int abilityIndex)
{
	if (m_abilitySuccess.Contains(abilityIndex))
	{
		return m_abilitySuccess[abilityIndex];
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
		for (const TPair<int, int> pair : m_abilityAttempt)
		{
			output += "Ability '" + FString::SanitizeFloat(pair.Key) + "' attempted '" + FString::SanitizeFloat(pair.Value) + "' times";
			output += "\n";
		}
	}

	if (m_abilitySuccess.Num() > 0)
	{
		for (const TPair<int, int> successPair : m_abilitySuccess)
		{
			output += "Ability '" + FString::SanitizeFloat(successPair.Key) + "' succeessfully landed '" + FString::SanitizeFloat(successPair.Value) + "' times";
			output += "\n";
		}
	}

	return output;
}

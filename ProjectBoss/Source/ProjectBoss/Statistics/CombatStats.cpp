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

void UCombatStats::AddDamageDealt(float amount)
{
	m_totalDealtDamage += amount;
}

float UCombatStats::GetTotalDamageDealt()
{
	return m_totalDealtDamage;
}

void UCombatStats::AddDamageRecieved(float amount)
{
	m_totalDamageRecieved += amount;
}

float UCombatStats::GetTotalDamageRecieved()
{
	return m_totalDamageRecieved;
}

void UCombatStats::AddHealthHealed(float amount)
{
	m_totalHealthHealed += amount;
}

float UCombatStats::GetTotalHealthHealed()
{
	return m_totalHealthHealed;
}

FString UCombatStats::GetAllStatsString()
{
	if (&m_abilityAttempt == nullptr || &m_abilitySuccess == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempt or Success is not valid!"));
		return "";
	}

	char NEW_LINE = '\n';

	FString output;
	// Add total and successful attacks
	output += "Total/Successful Attacks: " + FString::SanitizeFloat(GetTotalAttacks()) + " / " + FString::SanitizeFloat(GetSuccessfulAttacks());
	output += NEW_LINE;

	// Append dealt/recieved damage
	output += "Total Damage Dealt/Recieved: " + FString::SanitizeFloat(GetTotalDamageDealt()) + " / " + FString::SanitizeFloat(GetTotalDamageRecieved());
	output += NEW_LINE;

	// Add total health healed
	output += "Total Health healed: " + FString::SanitizeFloat(GetTotalHealthHealed());
	output += NEW_LINE;

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

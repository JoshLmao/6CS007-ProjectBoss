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

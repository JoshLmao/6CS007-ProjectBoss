// Fill out your copyright notice in the Description page of Project Settings.


#include "PBGOAPAction.h"
#include "Kismet/KismetMathLibrary.h"

UPBGOAPAction::UPBGOAPAction()
{
	m_timeToExecute = 0.0f;
	m_isInProgress = false;
	BossAbilityIndex = -1;		// Set to -1 on init, default value
}

bool UPBGOAPAction::doAction(APawn* p)
{
	bool result = Super::doAction(p);

	// If action in progress
	if (m_isInProgress && p)
	{
		UWorld* world = p->GetWorld();
		// Get delta time and increment
		float deltaTime = world->GetDeltaSeconds();
		m_timeToExecute += deltaTime;
	}

	return result;
}

FAtom UPBGOAPAction::CreateAtom(FString atmName, bool value)
{
	// Create atom, transfer data to it
	FAtom atom;
	atom.name = atmName;
	atom.value = value;
	return atom;
}

bool UPBGOAPAction::TrySetTarget(APawn* pawn)
{
	// Check pawn and type isnt null
	if (!pawn || !targetsType)
	{
		return false;
	}

	// Get target in world
	TArray<AActor*> targets = getTargetsList(pawn);
	if (targets.Num() <= 0)
	{
		return false;
	}
	 
	// Set targert of GOAP action to last target in list
	for (int i = 0; i < targets.Num(); i++)
	{
		setTarget(targets[i]);
		return true;
	}
	return false;
}

void UPBGOAPAction::UpdateCost(float newCost)
{
	if (newCost >= 0) {
		cost = newCost;
	} else {
		cost = 0.0f;
	}
}

float UPBGOAPAction::GetAverageExecuteTime()
{
	// Add up all execute times
	float total = 0.0f;
	for (float executeTime : allTimeToExecutes)
	{
		total += executeTime;
	}

	// Divide by total for average
	return total / allTimeToExecutes.Num();
}


void UPBGOAPAction::SetActionInProgress(bool inProgress)
{
	// If is already in progress & not in progress anymore & contains previous time...
	if (m_isInProgress && !inProgress && m_timeToExecute > 0)
	{
		// ...save the last time to execute
		allTimeToExecutes.Add(m_timeToExecute);
	}
	// ...else if not in progress & will be in progress...
	else if (!m_isInProgress && inProgress)
	{
		// ...set to 0 to begin count
		m_timeToExecute = 0.0f;
	}

	// Set flag
	m_isInProgress = inProgress;
}

float UPBGOAPAction::GetDamage() 
{
	return Damage;
}

int UPBGOAPAction::GetAbilityIndex()
{
	return BossAbilityIndex;
}

float UPBGOAPAction::GetBaseCost()
{
	return BaseCost;
}

// Fill out your copyright notice in the Description page of Project Settings.


#include "PBGOAPAction.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../ProjectBoss.h"
#include "../../Boss/BossCharacter.h"

UPBGOAPAction::UPBGOAPAction()
{
	m_timeToExecute = 0.0f;
	m_isInProgress = false;
	BossAbilityIndex = EAbilities::Any;		// Set to -1 on init, default value
	m_didSucceed = false;
	m_eventBound = false;
}

bool UPBGOAPAction::checkProceduralPrecondition(APawn* pawn)
{
	bool result = Super::checkProceduralPrecondition(pawn);

	return result;
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

	ABossCharacter* boss = Cast<ABossCharacter>(p);
	// Listen to ability success event on Boss
	if (!m_eventBound)
	{
		boss->OnAbilitySucceeded.AddDynamic(this, &UPBGOAPAction::OnAbilitySucceeded);
		m_eventBound = true;
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

void UPBGOAPAction::UpdateCost(float additionalCost)
{
	// Simple add newCost. Allow for negative
	cost = BaseCost + additionalCost;
}

float UPBGOAPAction::GetAverageExecuteTime()
{
	// Check if data exists, return 0 if none
	if (m_performanceHistory.Num() <= 0) {
		return 0.0f;
	}

	// Add up all execute times in performance history
	float total = 0.0f;
	for (FActionPerformance perf : m_performanceHistory)
	{
		total += perf.TimeToExecute;
	}

	// Divide by total for average
	return total / m_performanceHistory.Num();
}

float UPBGOAPAction::GetLastExecuteTime()
{
	// If history has a result, return last return
	if (m_performanceHistory.Num() > 0)
	{
		return m_performanceHistory[m_performanceHistory.Num() - 1].TimeToExecute;
	}

	// Else return default value
	return 0.0f;
}

float UPBGOAPAction::GetSuccessRateAsPercentage()
{
	// If no history, just return 0
	if (m_performanceHistory.Num() <= 0)
	{
		return 0.0f;
	}

	// Add up all successes in the history
	int totalSuccess = 0;
	for (FActionPerformance perf : m_performanceHistory)
	{
		totalSuccess += perf.DidSucceed ? 1 : 0;
	}

	// Divide success by total history to get 0-1 success rate
	float decimal = totalSuccess / m_performanceHistory.Num();

	// * 100 to get percentage
	return decimal * 100;
}


void UPBGOAPAction::SetActionInProgress(bool inProgress)
{
	// If is already in progress & not in progress anymore & contains previous time...
	if (m_isInProgress && !inProgress && m_timeToExecute > 0)
	{
		/// Action End
		// ...save action's performance
		m_performanceHistory.Add(FActionPerformance(m_didSucceed, m_timeToExecute));

		// print in log for debug
		UE_LOG(LogGOAP, Log, TEXT("Completed '%s' ability. Stats: didSucceed='%s' timeToExecute='%f'"), *getName(), (m_didSucceed ? TEXT("true") : TEXT("false")), m_timeToExecute);
	}
	// ...else if not in progress & will be in progress...
	else if (!m_isInProgress && inProgress)
	{
		/// Action Begin

		// Set tracking vars to default values
		m_timeToExecute = 0.0f;
		m_didSucceed = false;

		// Log begin for debug
		UE_LOG(LogGOAP, Log, TEXT("Action '%s' beginning"), *getName());
	}

	// Set flag
	m_isInProgress = inProgress;
}

void UPBGOAPAction::SetDidSucceed(bool didSucceed)
{
	m_didSucceed = didSucceed;
}

bool UPBGOAPAction::GetIsInProgress()
{
	return m_isInProgress;
}

void UPBGOAPAction::SetLastPerformanceDidSucceed(bool didSucceed)
{
	// Check data exists
	if (m_performanceHistory.Num() > 0)
	{
		int index = m_performanceHistory.Num() - 1;
		// Only update if they are different values
		if (m_performanceHistory[index].DidSucceed != didSucceed)
		{
			m_performanceHistory[index].DidSucceed = didSucceed;
		}
	}
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

bool UPBGOAPAction::GetDidSucceed()
{
	if (m_performanceHistory.Num() > 0)
	{
		return m_performanceHistory[m_performanceHistory.Num() - 1].DidSucceed;
	}
	else
	{
		return m_didSucceed;
	}
}

void UPBGOAPAction::OnPreconditionExit(bool exitState)
{
	// If can't perform action...
	if (!exitState)
	{
		// ... but action was in progress
		if (GetIsInProgress())
		{
			// Set action to not in progress to save data
			SetActionInProgress(false);
			UE_LOG(LogGOAP, Log, TEXT("Updating '%s' to exit"), *getName());
		}
	}
}

void UPBGOAPAction::OnAbilitySucceeded(int abilityIndex)
{
	// Check index has been set to anything
	if (BossAbilityIndex == EAbilities::Any)
	{
		return;
	}

	// Check successful ability index is equal to this action's ability
	if (abilityIndex == BossAbilityIndex)
	{
		// If no longer in progress, set previous
		if (!GetIsInProgress())
		{
			SetLastPerformanceDidSucceed(true);
		}

		// Set succeed flag to true
		SetDidSucceed(true);
	}
}
// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SessionStats.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API ASessionStats : public AActor
{
	GENERATED_BODY()
public:
	ASessionStats();

	/*
	*	VARIABLES
	*/
private:
	// Total time in seconds the session has been going for
	float m_totalSessionTime;

	/*
	*	METHODS
	*/
public:
	// Get the total time in seconds this session has been played for
	float GetSessionTime();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};

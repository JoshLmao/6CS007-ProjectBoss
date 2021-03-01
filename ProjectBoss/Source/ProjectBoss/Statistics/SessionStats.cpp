// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionStats.h"
#include "../ProjectBoss.h"

ASessionStats::ASessionStats()
{
	m_totalSessionTime = 0.0f;

	// Set actor to tick
	PrimaryActorTick.bCanEverTick = true;
}


void ASessionStats::BeginPlay()
{
	Super::BeginPlay();
}

void ASessionStats::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	m_totalSessionTime += deltaTime;
}

void ASessionStats::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Print final session stats to log
	UE_LOG(LogTemp, Log, TEXT("Session Time: '%f' seconds"), GetSessionTime());
}

float ASessionStats::GetSessionTime()
{
	return m_totalSessionTime;
}
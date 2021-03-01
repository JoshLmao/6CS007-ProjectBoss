// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectBossGameMode.h"
#include "ProjectBossCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/BossFightHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Boss/BossCharacter.h"
#include "Statistics/SessionStats.h"

AProjectBossGameMode::AProjectBossGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ProjectBoss/Player/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = BP_HUD;
}

void AProjectBossGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("Project Boss - Session begin"));

	// Listen to death events from player and boss
	Player = UGameplayStatics::GetActorOfClass(GetWorld(), AProjectBossCharacter::StaticClass());
	Boss = UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass());

	// Create session stats actor
	m_sessionStats = GetWorld()->SpawnActor<ASessionStats>(ASessionStats::StaticClass(), FVector(), FRotator());
}

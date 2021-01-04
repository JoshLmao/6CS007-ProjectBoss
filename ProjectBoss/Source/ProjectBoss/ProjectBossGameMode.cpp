// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectBossGameMode.h"
#include "ProjectBossCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/BossFightHUD.h"

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

	// Listen to death events from player and boss
}

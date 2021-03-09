// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectBossGameMode.h"
#include "ProjectBossCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "UI/BossFightHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Boss/BossCharacter.h"
#include "Statistics/SessionStats.h"
#include "ProjectBossCharacter.h"
#include "Boss/BossCharacter.h"
#include "World/BossSpawnTrigger.h"
#include "SaveData/ProjectBossSaveGame.h"
#include "Helpers/ProjectVersionBlueprint.h"
#include "Player/WukongPlayerController.h"

AProjectBossGameMode::AProjectBossGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ProjectBoss/Player/BP_Player"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	HUDClass = BP_HUD;
	PlayerControllerClass = AWukongPlayerController::StaticClass();
}

ASessionStats* AProjectBossGameMode::GetSessionStats()
{
	return SessionStats;
}

void AProjectBossGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("Project Boss - Session begin"));

	// Listen to death events from player
	AActor* playerActor = UGameplayStatics::GetActorOfClass(GetWorld(), AProjectBossCharacter::StaticClass());
	if (playerActor)
	{
		Player = Cast<AProjectBossCharacter>(playerActor);
		Player->OnCharacterDied.AddDynamic(this, &AProjectBossGameMode::OnPlayerDeath);
	}

	// Get boss spawn trigger in world and listen to event
	AActor* spawnTrigger = UGameplayStatics::GetActorOfClass(GetWorld(), ABossSpawnTrigger::StaticClass());
	if (spawnTrigger)
	{
		ABossSpawnTrigger* trigger = Cast<ABossSpawnTrigger>(spawnTrigger);
		trigger->OnSpawnedBoss.AddDynamic(this, &AProjectBossGameMode::OnBossSpawned);
	}

	// Create session stats actor
	SessionStats = GetWorld()->SpawnActor<ASessionStats>(ASessionStats::StaticClass(), FVector(), FRotator());
}

void AProjectBossGameMode::OnPlayerDeath()
{
	OnGameOver(false);
}

void AProjectBossGameMode::OnBossDeath()
{
	OnGameOver(true);
}

void AProjectBossGameMode::OnBossSpawned(AActor* bossActor)
{
	// On spawn, check valid
	if (bossActor)
	{
		// Cast and listen to boss death event
		Boss = Cast<ABossCharacter>(bossActor);
		Boss->OnCharacterDied.AddDynamic(this, &AProjectBossGameMode::OnBossDeath);
	}
}

void AProjectBossGameMode::OnGameOver(bool didPlayerWin)
{
	AController* aCon = Player->GetController();
	if (IsValid(Player) && IsValid(aCon))
	{
		// Get HUD and set state to EndPlay
		APlayerController* pc = Cast<APlayerController>(aCon);
		ABossFightHUD* hud = Cast<ABossFightHUD>(pc->GetHUD());
		if (hud)
		{
			hud->SetHUDState(EHUDState::EndPlay);
			hud->SetFreeCursor(pc, true);
		}

		// Disable input from player
		Player->SetDisableLocomotion(true);
		aCon->DisableInput(pc);
	}	

	// Load save game and increment total games
	UProjectBossSaveGame* saveGame = UProjectVersionBlueprint::LoadSaveGame();
	saveGame->TotalGames += 1;

	// Increment if player did win
	if (didPlayerWin)
	{
		saveGame->PlayerWins += 1;
	}
	// Finally, save game once done
	bool success = UProjectVersionBlueprint::SaveGame(saveGame);
	if (!success)
	{
		UE_LOG(LogTemp, Error, TEXT("Error when saving game!"));
	}
	else
	{
		FString outputStr = UProjectBossSaveGame::BuildSaveGameLogString(saveGame);
		UE_LOG(LogTemp, Log, TEXT("---\nSave Game Stats:\n%s\n---\n"), *outputStr);
	}
}
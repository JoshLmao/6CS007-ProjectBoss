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
	// Get HUD and set to EndPlay
	ABossFightHUD* hud = GetHUD();
	if (hud)
	{
		hud->SetHUDState(EHUDState::EndPlay);

		APlayerController* pc = Cast<APlayerController>(Player->GetController());
		hud->SetFreeCursor(pc, true);
	}
}

void AProjectBossGameMode::OnBossDeath()
{
	// Get HUD and set state to EndPlay
	ABossFightHUD* hud = GetHUD();
	if (hud)
	{
		hud->SetHUDState(EHUDState::EndPlay);

		APlayerController* pc = Cast<APlayerController>(Player->GetController());
		hud->SetFreeCursor(pc, true);
	}
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

ABossFightHUD* AProjectBossGameMode::GetHUD()
{
	APlayerController* pc = Cast<APlayerController>(Player->GetController());
	ABossFightHUD* hud = Cast<ABossFightHUD>(pc->GetHUD());
	return hud;
}
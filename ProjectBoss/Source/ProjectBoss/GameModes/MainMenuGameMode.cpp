// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../SaveData/ProjectBossSaveGame.h"
#include "../Helpers/ProjectVersionBlueprint.h"

AMainMenuGameMode::AMainMenuGameMode()
{

}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("Project Boss - Main Menu Start"));

	// Get Player Controller
	APlayerController* pc = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (pc)
	{
		// Free mouse and ignore any move input
		pc->bShowMouseCursor = true;
		pc->SetIgnoreMoveInput(true);
	}

	// Get a saved game
	UProjectBossSaveGame* saveGame = UProjectVersionBlueprint::LoadSaveGame();
	if (saveGame)
	{
		// Build string and output to log
		FString outputStr = UProjectBossSaveGame::BuildSaveGameLogString(saveGame);
		UE_LOG(LogTemp, Log, TEXT("---\nMain Menu Save Game:\n%s\n---"), *outputStr);
	}
}

void AMainMenuGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UE_LOG(LogTemp, Log, TEXT("Project Boss - Main Menu End"));
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectVersionBlueprint.h"
#include "Kismet/GameplayStatics.h"
#include "../SaveData/ProjectBossSaveGame.h"

FString UProjectVersionBlueprint::GetProjectVersion()
{
	FString projectVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		projectVersion,
		GGameIni
	);
	return projectVersion;
}

UProjectBossSaveGame* UProjectVersionBlueprint::LoadSaveGame()
{
	UProjectBossSaveGame* pbSaveGame = nullptr;

	USaveGame* save = UGameplayStatics::LoadGameFromSlot("save", 0);
	if (save)
	{
		pbSaveGame = Cast<UProjectBossSaveGame>(save);
	}

	return pbSaveGame;
}

bool UProjectVersionBlueprint::SaveGame(class UProjectBossSaveGame* game)
{
	if (!IsValid(game))
	{
		return false;
	}

	bool result = UGameplayStatics::SaveGameToSlot(game, "save", 0);
	return result;
}
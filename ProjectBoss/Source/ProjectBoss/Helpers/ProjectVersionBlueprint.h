// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectVersionBlueprint.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UProjectVersionBlueprint : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	static FString GetProjectVersion();

	// Loads the Project Boss Save Game
	UFUNCTION(BlueprintCallable)
	static class UProjectBossSaveGame* LoadSaveGame();

	// Saves the passed game and returns if it was successfull
	UFUNCTION(BlueprintCallable)
	static bool SaveGame(class UProjectBossSaveGame* game);
};

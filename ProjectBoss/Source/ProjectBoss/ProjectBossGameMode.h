// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectBossGameMode.generated.h"

UCLASS(minimalapi)
class AProjectBossGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AProjectBossGameMode();

public:
	// HUD class to use for this game mode
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHUD> BP_HUD;

	// Current player actor
	class AProjectBossCharacter* Player;
	// Reference to boss actor
	class ABossCharacter* Boss;

	// Current session stats actor
	UPROPERTY(EditAnywhere)
	class ASessionStats* SessionStats;

public:
	UFUNCTION(BlueprintCallable)
	class ASessionStats* GetSessionStats();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnPlayerDeath();

	UFUNCTION()
	void OnBossDeath();

	UFUNCTION()
	void OnBossSpawned(AActor* bossActor);

	// Gets the current HUD reference from the Player Controller
	class ABossFightHUD* GetHUD();

};




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
	AActor* Player;
	// Reference to boss actor
	AActor* Boss;

	// Current session stats actor
	class ASessionStats* m_sessionStats;

protected:
	virtual void BeginPlay() override;

};




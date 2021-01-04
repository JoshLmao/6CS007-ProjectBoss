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

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AHUD> BP_HUD;
};




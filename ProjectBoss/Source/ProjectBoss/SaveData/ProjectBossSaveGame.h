// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "ProjectBossSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UProjectBossSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// Default constructor
	UProjectBossSaveGame();

	// If the user wishes to enable ML participation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic")
	bool MLParticipation;
};

// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WukongPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API AWukongPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AWukongPlayerController();

private:
	bool m_isPaused;

	/*
	*	METHODS
	*/
public:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// Sets if the pause menu is paused or not
	UFUNCTION(BlueprintCallable)
	void SetIsPaused(bool isPaused);

private:
	void OnPerformPause();
};

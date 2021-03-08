// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BossFightHUD.generated.h"

UENUM()
enum EHUDState
{
	Gameplay = 0	UMETA(DisplayName = "Gameplay"),
	EndPlay = 1		UMETA(DisplayName = "EndPlay"),
	PauseMenu = 2	UMETA(DisplayName = "PauseMenu"),
};

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API ABossFightHUD : public AHUD
{
	GENERATED_BODY()

public:
	ABossFightHUD();

	/*
	*	 VARIABLES
	*/
private:
	// All gameplay UI widgets
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UUserWidget>> GameplayUIWidgets;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UUserWidget>> PauseMenuWidgets;

	// All UI widgets used to use when boss or player dies (Play ended)
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UUserWidget>> EndPlayWidgets;

	// Hit Marker widget
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HitMarkerWidget;

	// List of created widgets used during gameplay
	UPROPERTY()
	TArray<class UUserWidget*> m_createdGameplayWidgets;
	// List of created end play widgets
	UPROPERTY()
	TArray<class UUserWidget*> m_createdEndPlayWidgets;
	// All widgets used in the pause menu
	UPROPERTY()
	TArray<class UUserWidget*> m_createdPauseWidgets;
	// Active hit marker user widget
	UPROPERTY()
	class UUserWidget* m_activeHitMarker;

	// Tracks the delay to destroy a hit marker
	FTimerHandle m_hitMarkerDelay;

	EHUDState m_currentState;

	/*
	*	 METHODS
	*/
public:
	// Adds an on-screen hitmarker around the crosshair
	void AddHitMarker();
	// Set the HUD state to show the correct UI elements
	void SetHUDState(EHUDState state);

	void SetFreeCursor(class APlayerController* pc, bool isFree);

protected:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

private:
	// Callback once hit marker delay time is complete
	void OnHitMarkerDelay();

	// Creates instances of widgets and returns them
	TArray<class UUserWidget*> CreateWidgets(TArray<TSubclassOf<class UUserWidget>> widgets);
	// Sets the passed widgets to be displayed or removed from viewport
	void DisplayWidgetsOnViewport(TArray<class UUserWidget*> widgets, bool isOnViewport);

};

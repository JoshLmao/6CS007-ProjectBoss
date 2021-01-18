// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BossFightHUD.generated.h"

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
	// All constant on screen widgets to add on initialize
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UUserWidget>> AllUIWidgets;

	// Hit Marker widget
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HitMarkerWidget;

	// Current list of created UI widgets that are constantly active on screen
	TArray<class UUserWidget*> m_createdWidgets;
	// Active hit marker user widget
	class UUserWidget* m_activeHitMarker;

	// Tracks the delay to destroy a hit marker
	FTimerHandle m_hitMarkerDelay;

	/*
	*	 METHODS
	*/
public:
	// Adds an on-screen hitmarker around the crosshair
	void AddHitMarker();

protected:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

private:
	// Callback once hit marker delay time is complete
	void OnHitMarkerDelay();

};

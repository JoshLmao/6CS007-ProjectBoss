// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFightHUD.h"
#include "Blueprint/UserWidget.h"

ABossFightHUD::ABossFightHUD()
{

}

void ABossFightHUD::BeginPlay()
{
	Super::BeginPlay();

	// Create all widgets and add gameplay ones to viewport
	m_createdGameplayWidgets = CreateWidgets(GameplayUIWidgets);
	m_createdEndPlayWidgets = CreateWidgets(EndPlayWidgets);
	m_createdPauseWidgets = CreateWidgets(PauseMenuWidgets);

	SetHUDState(EHUDState::Gameplay);
}

void ABossFightHUD::DrawHUD()
{
	Super::DrawHUD();
}

void ABossFightHUD::AddHitMarker()
{
	// If hit marker hasn't been created yet, create it
	if (!m_activeHitMarker)
	{
		m_activeHitMarker = CreateWidget<UUserWidget>(GetWorld(), HitMarkerWidget);
	}

	// Check hit marker is still valid
	if (IsValid(m_activeHitMarker))
	{
		// Dont add if already in viewport
		if (m_activeHitMarker->IsInViewport())
			return;

		// Add to viewport and await removal
		m_activeHitMarker->AddToViewport();

		float delaySeconds = 0.35f;
		GetWorldTimerManager().SetTimer(m_hitMarkerDelay, this, &ABossFightHUD::OnHitMarkerDelay, delaySeconds);
	}
}

void ABossFightHUD::SetHUDState(EHUDState state)
{
	m_currentState = state;

	switch (m_currentState)
	{
		case EHUDState::Gameplay:
		{
			DisplayWidgetsOnViewport(m_createdGameplayWidgets, true);
			DisplayWidgetsOnViewport(m_createdEndPlayWidgets, false);
			DisplayWidgetsOnViewport(m_createdPauseWidgets, false);
			break;
		}
		case EHUDState::EndPlay:
		{
			DisplayWidgetsOnViewport(m_createdGameplayWidgets, false);
			DisplayWidgetsOnViewport(m_createdEndPlayWidgets, true);
			DisplayWidgetsOnViewport(m_createdPauseWidgets, false);
			break;
		}
		case EHUDState::PauseMenu:
		{
			DisplayWidgetsOnViewport(m_createdGameplayWidgets, false);
			DisplayWidgetsOnViewport(m_createdEndPlayWidgets, false);
			DisplayWidgetsOnViewport(m_createdPauseWidgets, true);
			break;
		}
		default:
			UE_LOG(LogTemp, Error, TEXT("Unexpected HUD state"));
	}
}

void ABossFightHUD::OnHitMarkerDelay()
{
	// Remove from viewport after delay
	m_activeHitMarker->RemoveFromViewport();

	GetWorldTimerManager().ClearTimer(m_hitMarkerDelay);
}

TArray<UUserWidget*> ABossFightHUD::CreateWidgets(TArray<TSubclassOf<UUserWidget>> widgets)
{
	TArray<UUserWidget*> createdWidgets;
	// Create gameplay widgets 
	if (widgets.Num() > 0)
	{
		for (TSubclassOf<UUserWidget> widget : widgets)
		{
			// Create and add to array
			UUserWidget* createdWidget = CreateWidget<UUserWidget>(GetWorld(), widget);
			if (IsValid(createdWidget))
			{
				createdWidgets.Add(createdWidget);
			}
		}
	}

	return createdWidgets;
}

void ABossFightHUD::DisplayWidgetsOnViewport(TArray<UUserWidget*> widgets, bool isOnViewport)
{
	if (widgets.Num() <= 0)
	{
		return;
	}

	// Iterate through all widgets
	for (UUserWidget* widget : widgets)
	{
		if (!IsValid(widget))
			continue;

		// If want to be in viewport but isn't already
		if (isOnViewport && !widget->IsInViewport())
		{
			widget->AddToViewport();
		}
		// If is in viewport and don't want to be in viewport
		else if (widget->IsInViewport() && !isOnViewport)
		{
			widget->RemoveFromViewport();
		}
	}
}

void ABossFightHUD::SetFreeCursor(APlayerController* pc, bool isFree)
{
	if (IsValid(pc))
	{
		pc->bShowMouseCursor = isFree;
	}
}
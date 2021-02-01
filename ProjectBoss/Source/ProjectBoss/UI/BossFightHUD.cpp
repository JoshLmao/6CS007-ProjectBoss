// Fill out your copyright notice in the Description page of Project Settings.


#include "BossFightHUD.h"
#include "Blueprint/UserWidget.h"

ABossFightHUD::ABossFightHUD()
{

}

void ABossFightHUD::BeginPlay()
{
	Super::BeginPlay();

	if (AllUIWidgets.Num() > 0)
	{
		for (TSubclassOf<UUserWidget> widget : AllUIWidgets)
		{
			UUserWidget* createdWidget = CreateWidget<UUserWidget>(GetWorld(), widget);
			createdWidget->AddToViewport();

			m_createdWidgets.Add(createdWidget);
		}
	}
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

void ABossFightHUD::OnHitMarkerDelay()
{
	// Remove from viewport after delay
	m_activeHitMarker->RemoveFromViewport();

	GetWorldTimerManager().ClearTimer(m_hitMarkerDelay);
}
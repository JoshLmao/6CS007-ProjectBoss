// Fill out your copyright notice in the Description page of Project Settings.


#include "WukongPlayerController.h"
#include "../UI/BossFightHUD.h"

AWukongPlayerController::AWukongPlayerController()
{

}

void AWukongPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetIsPaused(false);
}

void AWukongPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		// Listen to pausing
		InputComponent->BindAction("Pause", IE_Released, this, &AWukongPlayerController::OnPerformPause);
	}
}

void AWukongPlayerController::OnPerformPause()
{
	SetIsPaused(!m_isPaused);
}

void AWukongPlayerController::SetIsPaused(bool isPaused)
{
	m_isPaused = isPaused;

	ABossFightHUD* bossHUD = Cast<ABossFightHUD>(GetHUD());
	// Transition to...
	if (m_isPaused)
	{
		// Pause
		this->bShowMouseCursor = true;

		this->SetPause(true);
		bossHUD->SetHUDState(EHUDState::PauseMenu);
	}
	else
	{
		// Un pause
		this->SetInputMode(FInputModeGameOnly());
		this->bShowMouseCursor = false;

		this->SetPause(false);
		bossHUD->SetHUDState(EHUDState::Gameplay);
	}
}
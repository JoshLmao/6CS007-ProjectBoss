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
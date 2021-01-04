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
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class UUserWidget>> AllUIWidgets;

	TArray<class UUserWidget*> m_createdWidgets;

	/*
	*	 METHODS
	*/

protected:
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

};

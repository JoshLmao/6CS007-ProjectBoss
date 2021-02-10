// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UBossWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	// Reference to boss character
	class ABossCharacter* m_bossCharacter;

public:
	// Gets the current health
	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetCurrentHealth();
	// Gets the total health
	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetTotalHealth();

	// Gets the current health as a percentage, between 0 and 100
	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetCurrentHealthAsPercent();

	// Gets the currently held reference to the boss character, can be null if not found yet
	UFUNCTION(BlueprintCallable, Category = "Boss")
	ABossCharacter* GetBossCharacter();

protected:
	// Called on constructiuon of Widget
	virtual void NativeConstruct() override;
	// Called every frame Widget is on screen
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};

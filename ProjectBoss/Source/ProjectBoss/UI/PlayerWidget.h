// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	class AProjectBossCharacter* m_player;
	class UWorld* m_world;

public:
	// Gets the cooldown of the basic melee attack, rounded to one decimal places
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetBasicAttackCooldown();
	// Gets the current cooldown of advanced attack, rounded to one decimal places
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetAdvancedAttackCooldown();
	// Gets the current cooldown of ability one, rounded to one decimal places
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetAbilityOneCooldown();
	// Gets the current cooldown of ability two, rounded to one decimal places
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetAbilityTwoCooldown();
	// Get the current stance of the player
	UFUNCTION(BlueprintCallable, Category = "Player")
	enum EStance GetStance();

	// Gets the current health, rounded to two decimal places
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetCurrentHealth();
	// Gets the total health
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetTotalHealth();
	// Gets the current health of the player as a percentage, 0 to 100
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetCurrentHealthAsPercent();

protected:
	// Called when Widget gets constructed
	virtual void NativeConstruct() override;

};

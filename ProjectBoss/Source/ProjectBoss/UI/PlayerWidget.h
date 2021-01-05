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
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetBasicAttackCooldown();
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetAdvancedAttackCooldown();
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetAbilityOneCooldown();
	UFUNCTION(BlueprintCallable, Category = "Player")
	enum EStance GetStance();

	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetTotalHealth();

	UFUNCTION(BlueprintCallable, Category = "Player")
	float GetCurrentHealthAsPercent();


protected:
	virtual void NativeConstruct() override;

private:
	float BeautifyNumber(float num);

};

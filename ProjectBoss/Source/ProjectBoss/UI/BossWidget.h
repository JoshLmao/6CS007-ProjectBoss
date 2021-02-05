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
	class ABossCharacter* m_bossCharacter;
	class UWorld* m_world;

public:
	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetTotalHealth();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	float GetCurrentHealthAsPercent();

	UFUNCTION(BlueprintCallable, Category = "Boss")
	ABossCharacter* GetBossCharacter();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};

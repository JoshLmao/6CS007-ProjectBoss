// Fill out your copyright notice in the Description page of Project Settings.


#include "BossWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../Boss/BossCharacter.h"

void UBossWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AActor* boss = UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass());
	m_bossCharacter = Cast<ABossCharacter>(boss);
}

void UBossWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Keep searching for boss until spawned
	if (!m_bossCharacter)
	{
		AActor* boss = UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass());
		if (boss)
		{
			m_bossCharacter = Cast<ABossCharacter>(boss);
		}
	}
}

float UBossWidget::GetCurrentHealth()
{
	if (m_bossCharacter)
		return m_bossCharacter->GetCurrentHealth();
	return 0.0f;
}

float UBossWidget::GetTotalHealth()
{
	if (m_bossCharacter)
		return m_bossCharacter->GetTotalHealth();
	return 0.0f;
}

float UBossWidget::GetCurrentHealthAsPercent()
{
	if (m_bossCharacter)
	{
		float current = m_bossCharacter->GetCurrentHealth();
		float total = m_bossCharacter->GetTotalHealth();
		float result = current / total;
		return result;
	}
	return 0.0f;
}

ABossCharacter* UBossWidget::GetBossCharacter()
{
	return m_bossCharacter;
}

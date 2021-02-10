// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../ProjectBossCharacter.h"
#include "../UtilityHelper.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Get reference to player actor on construct
	AActor* player = UGameplayStatics::GetActorOfClass(GetWorld(), AProjectBossCharacter::StaticClass());
	m_player = Cast<AProjectBossCharacter>(player);
}

float UPlayerWidget::GetBasicAttackCooldown()
{
	if (m_player)
	{
		return UtilityHelper::BeautifyNumber(m_player->MeleeAtkCurrentCd);
	}
	return 0.0f;
}

float UPlayerWidget::GetAdvancedAttackCooldown()
{
	if (m_player)
	{
		return UtilityHelper::BeautifyNumber(m_player->AdvAttackCurrentCd);
	}
	return 0.0f;
}

float UPlayerWidget::GetAbilityOneCooldown()
{
	if (m_player)
	{
		return UtilityHelper::BeautifyNumber(m_player->AbilOneCurrentCd);
	}
	return 0.0f;
}

float UPlayerWidget::GetAbilityTwoCooldown()
{
	if (m_player)
	{
		return UtilityHelper::BeautifyNumber(m_player->AbilTwoCurrentCd);
	}
	return 0.0f;
}

EStance UPlayerWidget::GetStance()
{
	if (m_player)
	{
		return m_player->GetStance();
	}
	return EStance::Offensive;
}

float UPlayerWidget::GetCurrentHealth()
{
	if (m_player)
		return UtilityHelper::BeautifyNumber(m_player->GetCurrentHealth());
	return 0.0f;
}

float UPlayerWidget::GetTotalHealth()
{
	if (m_player)
		return m_player->GetTotalHealth();
	return 0.0f;
}

float UPlayerWidget::GetCurrentHealthAsPercent()
{
	if (m_player)
	{
		float current = m_player->GetCurrentHealth();
		float total = m_player->GetTotalHealth();
		float result = current / total;
		return result;
	}
	return 0.0f;
}
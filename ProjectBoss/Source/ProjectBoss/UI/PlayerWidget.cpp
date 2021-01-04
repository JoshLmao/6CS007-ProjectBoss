// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWidget.h"
#include "Kismet/GameplayStatics.h"
#include "../ProjectBossCharacter.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AActor* player = UGameplayStatics::GetActorOfClass(GetWorld(), AProjectBossCharacter::StaticClass());
	m_player = Cast<AProjectBossCharacter>(player);
}

float UPlayerWidget::BeautifyNumber(float num)
{
	float val = FMath::RoundHalfFromZero(num);
	if (val < 0)
		val = 0;
	return val;
}

float UPlayerWidget::GetBasicAttackCooldown()
{
	return 0.0f;
}

float UPlayerWidget::GetAdvancedAttackCooldown()
{
	if (m_player)
	{
		return BeautifyNumber(m_player->AdvAttackCurrentCd);
	}
	return 0.0f;
}

float UPlayerWidget::GetAbilityOneCooldown()
{
	if (m_player)
	{
		return BeautifyNumber(m_player->AbilOneCurrentCd);
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
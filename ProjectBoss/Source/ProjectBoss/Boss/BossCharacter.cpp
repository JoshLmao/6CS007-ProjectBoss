// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
ABossCharacter::ABossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_attackCount = 0;
	m_isAttacking = false;
	m_saveAttack = false;

	TotalHealth = 2500.0f;
}

// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetActorLabel("Kalari");
	CurrentHealth = TotalHealth;
}

// Called every frame
void ABossCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	
}

// Called to bind functionality to input
void ABossCharacter::SetupPlayerInputComponent(UInputComponent* playerInputComponent)
{
	Super::SetupPlayerInputComponent(playerInputComponent);

}

float ABossCharacter::TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser)
{
	float damage = Super::TakeDamage(damageAmount, damageEvent, eventInstigator, damageCauser);

	CurrentHealth -= damage;

	UE_LOG(LogTemp, Log, TEXT("Boss Health: %f"), CurrentHealth);
	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Boss has died!"));
		CurrentHealth = 0;

		OnDeath();
	}

	return damage;
}

void ABossCharacter::PerformMeleeAttack()
{
	if (m_isAttacking)
	{
		m_saveAttack = true;
	}
	else
	{
		m_isAttacking = true;

		this->PlayAnimMontage(AttackAnimMontages[m_attackCount]);
		m_attackCount++;
		if (m_attackCount >= AttackAnimMontages.Num())
		{
			m_attackCount = 0;
		}
	}
}

void ABossCharacter::ComboAttackSave()
{
	// Only combo if saveAttack has been set to true (called PerformMeleeAttack again)
	if (m_saveAttack)
	{
		m_saveAttack = false;

		this->PlayAnimMontage(AttackAnimMontages[m_attackCount]);

		m_attackCount++;
		if (m_attackCount >= AttackAnimMontages.Num())
		{
			m_attackCount = 0;
		}
	}
}

void ABossCharacter::ResetCombo()
{
	m_attackCount = 0;
	m_saveAttack = false;
	m_isAttacking = false;
}

float ABossCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}

float ABossCharacter::GetTotalHealth()
{
	return TotalHealth;
}

void ABossCharacter::OnDeath()
{
	UE_LOG(LogTemp, Log, TEXT("BossCharacter has died!"));

	if (OnCharacterDied.IsBound())
		OnCharacterDied.Broadcast();

	DetachFromControllerPendingDestroy();

	GetMovementComponent()->StopMovementImmediately();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	SetActorEnableCollision(true);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent());
	if (CharacterComp)
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}
}

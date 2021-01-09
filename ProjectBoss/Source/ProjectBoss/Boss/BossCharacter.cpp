// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/MeshComponent.h"
#include "../ProjectBossCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include "ThrowableDagger.h"
#include "Navigation/PathFollowingComponent.h"
#include "AIController.h"
#include "Materials/MaterialInstanceDynamic.h"

// Sets default values
ABossCharacter::ABossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_attackCount = 0;
	m_isAttacking = false;
	m_saveAttack = false; 
	m_dmgThisAttack = false;

	TotalHealth = 2500.0f;
	MeleeDamage = 50.0f;

	m_rmbCurrentCooldown = 0.0f;
	AdvAbilityDamage = 50.0f;
	AdvAbilityCooldown = 20.0f;

	UltimateDamage = 75.0f;
	m_ultiCurrentCooldown = 0.0f;
	UltimateCooldown = 50.0f;

	LeftBladeCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftBladeCollider"));
	LeftBladeCollider->SetupAttachment(GetMesh());
	LeftBladeCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	LeftBladeCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	LeftBladeCollider->SetGenerateOverlapEvents(true);
	LeftBladeCollider->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnBladeBeginOverlap);
	LeftBladeCollider->SetHiddenInGame(false);

	RightBladeCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightBladeCollider"));
	RightBladeCollider->SetupAttachment(GetMesh());
	RightBladeCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RightBladeCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	RightBladeCollider->SetGenerateOverlapEvents(true);
	RightBladeCollider->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnBladeBeginOverlap);
	RightBladeCollider->SetHiddenInGame(false);
}

// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetActorLabel("Kalari");
	CurrentHealth = TotalHealth;

	m_aiController = Cast<AAIController>(GetController());

	if (LeftBladeCollider)
	{
		LeftBladeCollider->AttachTo(GetMesh(), "sword_handle_l", EAttachLocation::SnapToTarget, false);
		LeftBladeCollider->SetCapsuleHalfHeight(40.0f);
		LeftBladeCollider->SetCapsuleRadius(10.0f);
		LeftBladeCollider->AddLocalRotation(FRotator(90.0f, 0, 0));
		LeftBladeCollider->AddLocalOffset(FVector(0, 0, -30.0f));
	}
	if (RightBladeCollider)
	{
		RightBladeCollider->AttachTo(GetMesh(), "sword_handle_r", EAttachLocation::SnapToTarget, false);
		RightBladeCollider->SetCapsuleHalfHeight(40.0f);
		RightBladeCollider->SetCapsuleRadius(10.0f);
		RightBladeCollider->AddLocalRotation(FRotator(90.0f, 0, 0));
		RightBladeCollider->AddLocalOffset(FVector(0, 0, 30.0f));
	}

	//m_originalMeshMaterials = GetMesh()->GetMaterials();
	for (int i = 0; i < GetMesh()->GetNumMaterials(); i++) {
		m_originalMeshMaterials.Add(GetMesh()->CreateDynamicMaterialInstance(i));
	}

	m_invisMatInst = GetMesh()->CreateDynamicMaterialInstance(0, InvisibleMatInst);
}

// Called every frame
void ABossCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);


	if (m_ultiCurrentCooldown > 0)
		m_ultiCurrentCooldown -= deltaTime;

	if (CurrentHealth > 0)
	{
		// Look at player when aiming to throw RMB attack
		if (IsValid(m_rmbTarget))
		{
			m_aiController->MoveToActor(m_rmbTarget, 1500.0f);
			LookAtActor(m_rmbTarget);
		}

		// Face actor towards target jump actor
		if (m_ultiIsChanneling && m_ultiTargetActor)
		{
			LookAtActor(m_ultiTargetActor);
		}
	}
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

void ABossCharacter::PerformAdvancedAttack(AActor* target)
{
	if (!target)
		return;

	UE_LOG(LogTemp, Log, TEXT("Boss performing Advanced Attack"));

	m_rmbTarget = target;

	this->PlayAnimMontage(AdvancedAttackMontages[0]);
}

void ABossCharacter::SetInvisible(bool isInvis)
{
	if (isInvis)
	{
		for (int i = 0; i < GetMesh()->GetNumMaterials(); i++) 
		{
			GetMesh()->SetMaterial(i, m_invisMatInst);
		}
	}
	else
	{
		for (int i = 0; i < GetMesh()->GetNumMaterials(); i++) 
		{
			GetMesh()->SetMaterial(i, m_originalMeshMaterials[i]);
		}
	}
}

bool ABossCharacter::ChaseTarget(AActor* target)
{
	LookAtActor(target);

	EPathFollowingRequestResult::Type type = m_aiController->MoveToActor(target);
	return type == EPathFollowingRequestResult::Type::AlreadyAtGoal;
}

void ABossCharacter::AdvAttackFinishedPrepare()
{
	// Finish prepare anim, start rand delay to throw dagger

	float randDuration = FMath::RandRange(0.75f, 3.25f);
	GetWorldTimerManager().SetTimer(m_rmbDelayHandle, this, &ABossCharacter::AdvAttackOnThrowDagger, randDuration, false);
	UE_LOG(LogTemp, Log, TEXT("Delaying RMB Throw by '%f' seconds"), randDuration);

	// Aiming anim length max is 9.1 seconds
	if (AdvancedAttackMontages.Num() > 2)
	{
		this->PlayAnimMontage(AdvancedAttackMontages[1]);
	}
}

void ABossCharacter::AdvAttackOnThrowDagger()
{
	// Waited random time, play throw anim
	if (AdvancedAttackMontages.Num() > 2)
	{
		this->PlayAnimMontage(AdvancedAttackMontages[2]);
	}
}

void ABossCharacter::AdvAttackOnReleaseDagger()
{
	if (!ThrowableDagger)
	{
		UE_LOG(LogTemp, Error, TEXT("Unable to create dagger since no dagger has been set in Editor"));
		return;
	}

	// Spawnloc: at actors location + (small amount of units in front) + at arm's height
	FVector spawnLoc = this->GetActorLocation() + (this->GetActorRightVector() * 100.0f) + FVector(0, 0, 50.0f);
	// On throw anim release dagger. Spawn dagger with velocity
	AThrowableDagger* daggerActor = GetWorld()->SpawnActor<AThrowableDagger>(ThrowableDagger.Get(), spawnLoc, FRotator());
	if (daggerActor)
	{
		AThrowableDagger* dagger = Cast<AThrowableDagger>(daggerActor);
		dagger->SetDamage(AdvAbilityDamage);
		
		dagger->SetActorLocation(spawnLoc);

		FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(dagger->GetActorLocation(), m_rmbTarget->GetActorLocation());
		FRotator daggerRot = dagger->GetActorRotation();
		dagger->SetActorRotation(lookAt + FRotator(-90, 0, 0));

		dagger->SetMovementDirection(dagger->GetActorUpVector(), 3000.0f);
	}
}

void ABossCharacter::PerformUltimate(AActor* targetActor)
{
	if (!targetActor || m_ultiIsChanneling)
		return;

	UE_LOG(LogTemp, Log, TEXT("Boss performing Ultimate!"));

	m_ultiTargetActor = targetActor;

	// Play channel montage for ultimate
	if (AbilityUltimateMontages.Num() > 0)
	{
		this->PlayAnimMontage(AbilityUltimateMontages[0]);
	}

	m_ultiIsChanneling = true;
}

void ABossCharacter::UltimateTeleportTo()
{
	if (!m_ultiTargetActor)
		return;

	if (AbilityUltimateMontages.Num() > 1)
	{
		this->PlayAnimMontage(AbilityUltimateMontages[1]);
	}

	// Teleport boss to character
	FVector directionVector = UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), m_ultiTargetActor->GetActorLocation());
	UE_LOG(LogTemp, Log, TEXT("DirVector: %s"), *directionVector.ToString());
	FVector targetLocation = m_ultiTargetActor->GetActorLocation() - (directionVector * 100.0f);

	this->SetActorLocation(targetLocation);

	// Only apply damage if player didn't "avoid" it
	AProjectBossCharacter* player = Cast<AProjectBossCharacter>(m_ultiTargetActor);
	if (!player->IsEvading())
	{
		m_ultiTargetActor->TakeDamage(UltimateDamage, FDamageEvent(), GetController(), this);
	}

	m_ultiIsChanneling = false;
	m_ultiCurrentCooldown = UltimateCooldown;
	m_ultiTargetActor = false;
}

void ABossCharacter::ComboAttackSave()
{
	// Only combo if saveAttack has been set to true (called PerformMeleeAttack again)
	if (m_saveAttack)
	{
		m_saveAttack = false;
		m_dmgThisAttack = false;

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
	m_dmgThisAttack = false;
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

void ABossCharacter::OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AProjectBossCharacter::StaticClass()) && m_isAttacking && !m_dmgThisAttack)
	{
		FDamageEvent dmgEvent;
		OtherActor->TakeDamage(MeleeDamage, dmgEvent, GetController(), this);
		m_dmgThisAttack = true;
	}
}

float ABossCharacter::GetUltimateCooldown()
{
	return m_ultiCurrentCooldown;
}

void ABossCharacter::LookAtActor(AActor* target)
{
	// Set Actor to look at target actor
	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), target->GetActorLocation());
	FRotator actorRotation = this->GetActorRotation();
	actorRotation.Yaw = lookAt.Yaw + -90.0f;
	this->SetActorRotation(actorRotation);

	// Also set Controller's look rotation
	if (GetController()) 
	{
		GetController()->SetControlRotation(actorRotation);
	}
}
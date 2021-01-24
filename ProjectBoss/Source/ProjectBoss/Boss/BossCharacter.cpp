// Fill out your copyright notice in the Description page of Project Settings.


#include "BossCharacter.h"
#include "../ProjectBoss.h"
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
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"

// Sets default values
ABossCharacter::ABossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_isPerformingAnyAbility = false;
	// General
	TotalHealth = 2500.0f;
	// Melee Attack
	m_attackCount = 0;
	m_isAttacking = false;
	m_saveAttack = false;
	m_dmgThisAttack = false;
	AttackRate = 1.25f;
	MeleeDamage = 50.0f;
	MeleeRadius = 150.0f;
	// Ability Advanced
	AdvAbilityDamage = 50.0f;
	AdvAbilityTotalCooldown = 20.0f;
	AdvAbilityCurrentCd = 0.0f;
	// Ability One
	AbilOneCritAmount = 150.0f;
	AbilOneTotalCooldown = 10.0f;
	AdvAbilityCurrentCd = 0.0f;
	m_isInvisible = false;
	// Ability Ultimate
	UltimateDamage = 150.0f;
	UltimateTotalCooldown = 50.0f;
	UltimateCurrentCd = 0.0f;
	// Ability Heal
	m_healTimeRemaining = 0.0f;
	HealDuration = 5.0f;
	HealPercent = 0.17;
	HealTotalCooldown = 30.0f;
	PerformHealThreshold = 0.35f;

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

	m_bossAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	m_bossAudioComponent->bAutoActivate = false;
	m_bossAudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set actor label & start health
	SetActorLabel("Kallari");
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
		m_originalMeshMaterials.Add(GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(i, GetMesh()->GetMaterial(i)));
	}

	m_invisMatInst = GetMesh()->CreateDynamicMaterialInstance(0, InvisibleMatInst);
}

// Called every frame
void ABossCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Lower any ability cooldowns
	UpdateAbilityCooldowns(deltaTime);

	// If stunned, disable any in movement/look at actions
	if (IsStunned)
		return;

	// While Boss is alive, perform any abilities
	if (CurrentHealth > 0)
	{
		// Look at player when aiming to throw RMB attack
		if (IsValid(m_rmbTarget) && IsValid(m_aiController))
		{
			m_aiController->MoveToActor(m_rmbTarget, 1500.0f);
			LookAtActor(m_rmbTarget->GetActorLocation());
		}

		// Face actor towards target jump actor
		if (m_ultiIsChanneling && IsValid(m_ultiTargetActor))
		{
			LookAtActor(m_ultiTargetActor->GetActorLocation());
		}

		// Heal ability if time still remaining
		if (m_healTimeRemaining > 0)
		{
			// Decrement time remaining
			m_healTimeRemaining -= deltaTime;

			// Determine amount to heal this tick
			float totalHealAmount = GetTotalHealth() * HealPercent;
			float oneSec = totalHealAmount / HealDuration;
			float healAmount = oneSec * deltaTime;
			CurrentHealth += healAmount;

			// If current health is full, stop heal
			if (CurrentHealth >= TotalHealth)
			{
				CurrentHealth = TotalHealth;
				m_healTimeRemaining = 0;
			}
			
			// Stop Heal montage once heal over
			if (m_healTimeRemaining <= 0)
			{
				this->StopAnimMontage(HealMontage);

				OnFinishHeal();
			}
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

	UE_LOG(LogBoss, Log, TEXT("Boss takes '%f' damage from '%s' (Total Health: '%f')"), damage, *damageCauser->GetName(), CurrentHealth);

	if (CurrentHealth <= 0)
	{
		UE_LOG(LogBoss, Log, TEXT("Boss has died!"));
		CurrentHealth = 0;

		OnDeath();
	}

	return damage;
}

void ABossCharacter::PerformMeleeAttack()
{
	if (IsPerformingAbility())
		return;

	if (m_isAttacking)
	{
		m_saveAttack = true;
	}
	else
	{
		m_isAttacking = true;

		this->PlayAnimMontage(AttackAnimMontages[m_attackCount], 1 / AttackRate);
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
		m_dmgThisAttack = false;

		this->PlayAnimMontage(AttackAnimMontages[m_attackCount], 1 / AttackRate);

		m_attackCount++;
		if (m_attackCount >= AttackAnimMontages.Num())
		{
			m_attackCount = 0;
		}
	}
}

void ABossCharacter::PerformAdvancedAttack(AActor* target)
{
	if (!target || m_isPerformingAnyAbility)
		return;

	UE_LOG(LogBoss, Log, TEXT("Boss performing Advanced Attack"));

	m_rmbTarget = target;
	m_isPerformingAnyAbility = true;

	this->PlayAnimMontage(AdvancedAttackMontages[0]);
}

void ABossCharacter::SetInvisible(bool isInvis)
{
	m_isInvisible = isInvis;

	if (IsValid(GetMesh()) && m_originalMeshMaterials.Num() > 0)
	{
		for (int i = 0; i < m_originalMeshMaterials.Num(); i++)
		{
			if (isInvis)
			{
				GetMesh()->SetMaterial(i, m_invisMatInst);
			}
			else
			{
				if (IsValid(m_originalMeshMaterials[i]) && m_originalMeshMaterials[i]->IsValidLowLevel())
					GetMesh()->SetMaterial(i, m_originalMeshMaterials[i]);
				else
					UE_LOG(LogBoss, Error, TEXT("Material '%d' invalid!"), i);
			}
		}
	}
}

bool ABossCharacter::MoveToLocation(FVector location)
{
	/*if (!IsValid(location))
	{
		return false;
	}*/

	// Look and face towards target
	LookAtActor(location);

	// Check if aiController is valid and move towards
	if (IsValid(m_aiController))
	{
		// Configure move request
		FAIMoveRequest aiRequest;
		aiRequest.SetGoalLocation(location);
		aiRequest.SetAcceptanceRadius(MeleeRadius / 1.25);

		// Set controller to move to target
		EPathFollowingRequestResult::Type type = m_aiController->MoveTo(aiRequest);
		return type == EPathFollowingRequestResult::Type::AlreadyAtGoal;
	}
	return false;
}

void ABossCharacter::CancelMoveToLocation()
{
	if (IsValid(m_aiController)) 
	{
		// Stop Movement of controller
		m_aiController->StopMovement();
	}
}

void ABossCharacter::AdvAttackFinishedPrepare()
{
	// Finish prepare anim, start rand delay to throw dagger

	float randDuration = FMath::RandRange(0.75f, 3.25f);
	GetWorldTimerManager().SetTimer(m_rmbDelayHandle, this, &ABossCharacter::AdvAttackOnThrowDagger, randDuration, false);
	UE_LOG(LogBoss, Log, TEXT("Delaying RMB Throw by '%f' seconds"), randDuration);

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
		UE_LOG(LogBoss, Error, TEXT("Unable to create dagger since no dagger has been set in Editor"));
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

	m_rmbTarget = nullptr;
	m_isPerformingAnyAbility = false;
	
	// On cooldown once ability finished performing
	AdvAbilityCurrentCd = AdvAbilityTotalCooldown;
}

void ABossCharacter::PerformUltimate(AActor* targetActor)
{
	if (!targetActor || !CanPerformAction())
	{
		return;
	}

	UE_LOG(LogBoss, Log, TEXT("Performing ultimate ability!"));

	m_ultiTargetActor = targetActor;
	m_isPerformingAnyAbility = true;

	// Play channel montage for ultimate
	if (AbilityUltimateMontages.Num() > 0)
	{
		this->PlayAnimMontage(AbilityUltimateMontages[0]);
	}

	m_ultiIsChanneling = true;
}

void ABossCharacter::PerformHeal()
{
	if (!CanPerformAction())
	{
		return;
	}

 	UE_LOG(LogBoss, Log, TEXT("Performing Heal ability at '%f' health for '%f' percent"), CurrentHealth, HealPercent);
	m_isPerformingAnyAbility = true;

	if (HealMontage)
	{
		this->PlayAnimMontage(HealMontage);
	}

	m_healTimeRemaining = HealDuration;
}

void ABossCharacter::OnFinishHeal()
{
	UE_LOG(LogBoss, Log, TEXT("Finished Heal ability to '%f' health. Cooldown for '%f' seconds"), CurrentHealth, HealTotalCooldown);

	HealCurrentCooldown = HealTotalCooldown;
	m_healTimeRemaining = 0;
	
	m_isPerformingAnyAbility = false;
}

bool ABossCharacter::CanHeal()
{
	return CurrentHealth < (TotalHealth * PerformHealThreshold) && HealCurrentCooldown <= 0;
}

void ABossCharacter::UltimateTeleportTo()
{
	if (!m_ultiTargetActor)
	{
		UE_LOG(LogPlayer, Error, TEXT("Unable to teleport on Ultimate! No Target actor set"));
		return;
	}

	//  Determine teleport location next to target
	FVector directionVector = UKismetMathLibrary::GetDirectionUnitVector(this->GetActorLocation(), m_ultiTargetActor->GetActorLocation());
	UE_LOG(LogBoss, Log, TEXT("DirVector: %s"), *directionVector.ToString());
	FVector targetLocation = m_ultiTargetActor->GetActorLocation() - (directionVector * 100.0f);
	// Teleport actor to location
	this->SetActorLocation(targetLocation);

	// Only apply damage if player didn't "avoid"/evade it
	AProjectBossCharacter* player = Cast<AProjectBossCharacter>(m_ultiTargetActor);
	if (!player->IsEvading())
	{
		m_ultiTargetActor->TakeDamage(UltimateDamage, FDamageEvent(), GetController(), this);
	}

	// Play ultimate montage to attack player
	if (AbilityUltimateMontages.Num() > 1)
	{
		this->PlayAnimMontage(AbilityUltimateMontages[1]);
	}

	// Set cooldown
	UltimateCurrentCd = UltimateTotalCooldown;

	// Reset flags
	m_ultiIsChanneling = false;
	m_ultiTargetActor = false;
	m_isPerformingAnyAbility = false;
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

float ABossCharacter::GetAdvancedAbilityCooldown()
{
	return AdvAbilityCurrentCd;
}

float ABossCharacter::GetAbilityOneCooldown()
{
	return AbilOneCurrentCd;
}

void ABossCharacter::OnDeath()
{
	UE_LOG(LogBoss, Log, TEXT("BossCharacter has died!"));

	if (OnCharacterDied.IsBound())
		OnCharacterDied.Broadcast();

	if (m_bossAudioComponent)
	{
		m_bossAudioComponent->SetSound(DeathCue);
		m_bossAudioComponent->Play();
	}

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
		float dmgAmount = MeleeDamage;
		// If performing certain ability that changes attack damage, set to that
		if (MeleeCritMultiplier > 0)
		{
			dmgAmount = MeleeDamage * (MeleeCritMultiplier / 100);
			MeleeCritMultiplier = 0;
		}

		FDamageEvent dmgEvent;
		OtherActor->TakeDamage(dmgAmount, dmgEvent, GetController(), this);
		m_dmgThisAttack = true;
	}
}

void ABossCharacter::UpdateAbilityCooldowns(float deltaTime)
{
	if (AdvAbilityCurrentCd > 0)
		AdvAbilityCurrentCd -= deltaTime;
	if (AbilOneCurrentCd > 0)
		AbilOneCurrentCd -= deltaTime;
	if (UltimateCurrentCd > 0)
		UltimateCurrentCd -= deltaTime;
	if (HealCurrentCooldown > 0)
		HealCurrentCooldown -= deltaTime;
}

float ABossCharacter::GetUltimateCooldown()
{
	return UltimateCurrentCd;
}

void ABossCharacter::LookAtActor(FVector location)
{
	if (!IsValid(m_aiController))
	{
		UE_LOG(LogBoss, Error, TEXT("Can't perform LookAtActor!"));
		return;
	}

	// Set Actor to look at target actor
	FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), location);
	FRotator actorRotation = this->GetActorRotation();
	actorRotation.Yaw = lookAt.Yaw + -90.0f;
	this->SetActorRotation(actorRotation);

	// Also set Controller's look rotation
	m_aiController->SetControlRotation(actorRotation);
}

float ABossCharacter::GetMeleeRadius()
{
	return MeleeRadius;
}

void ABossCharacter::ApplyStun(float duration)
{
	// Play the stun montage
	if (StunnedMontage)
	{
		this->PlayAnimMontage(StunnedMontage);
	}
	// Set IsStunned
	IsStunned = true;
	GetWorldTimerManager().SetTimer(m_stunHandle, this, &ABossCharacter::EndStun, duration, false);

	UE_LOG(LogBoss, Log, TEXT("Boss is stunned for '%f' seconds"), duration);
}

float ABossCharacter::GetAbilityOneCritMultiplier()
{
	return AbilOneCritAmount;
}

void ABossCharacter::EndStun()
{
	// On stun end, stop playing the stun montage
	IsStunned = false;
	this->StopAnimMontage(StunnedMontage);

	UE_LOG(LogBoss, Log, TEXT("No longer stunned"));
}

bool ABossCharacter::CanPerformAction()
{
	if (m_isAttacking || m_isPerformingAnyAbility || m_ultiIsChanneling)
	{
		return false;
	}
	return true;
}

bool ABossCharacter::GetIsStunned()
{
	return IsStunned;
}

bool ABossCharacter::IsPerformingAbility(int abilIndex)
{
	switch (abilIndex)
	{
		case EAbilities::Melee:
			return m_isAttacking;
		case EAbilities::Advanced:
			return IsValid(m_rmbTarget);
		case EAbilities::One:
			return m_isInvisible;
		case EAbilities::Ultimate:
			return IsValid(m_ultiTargetActor);
		case EAbilities::Heal:
			return m_healTimeRemaining > 0;
		default:
			return m_isPerformingAnyAbility;
	}
}

void ABossCharacter::BeginAbilityOneCooldown()
{
	AbilOneCurrentCd = AbilOneTotalCooldown;
}

float ABossCharacter::GetHealCooldown()
{
	return HealCurrentCooldown;
}

void ABossCharacter::SetMeleeCritMultiplier(float percentMultiplier)
{
	MeleeCritMultiplier = percentMultiplier;
}

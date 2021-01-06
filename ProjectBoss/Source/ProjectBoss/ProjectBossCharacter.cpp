// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectBossCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Boss\BossCharacter.h"
#include "Components/SphereComponent.h"
#include "Player\CapsuleAOEDamage.h"
#include "Player/CloudwalkCloud.h"

//////////////////////////////////////////////////////////////////////////
// AProjectBossCharacter

AProjectBossCharacter::AProjectBossCharacter()
{
	TotalHealth = 2000.0f;
	CurrentHealth = TotalHealth;

	MeleeAttackCooldown = 1.0f;

	AdvAttackCurrentCd = 0.0f;
	AdvAttackOffensiveTotalCooldown = 10.0f;
	AdvAttackOffensiveDamageAmount = 250.0f;

	AbilityOneTotalCooldown = 10.0f;
	AbilityOneRadius = 500.0f;
	AbilOneCurrentCd = 0.0f;
	AbilityOneDamageAmount = 100.0f;

	m_hasAttackedThisSwing = false;

	CurrentStance = EStance::Offensive;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	PoleColliderComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PoleColliderComponent"));
	PoleColliderComponent->SetupAttachment(RootComponent);

	PoleColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectBossCharacter::OnPoleBeginOverlap);
	PoleColliderComponent->OnComponentEndOverlap.AddDynamic(this, &AProjectBossCharacter::OnPoleEndOverlap);

	PoleColliderComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PoleColliderComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	PoleColliderComponent->SetGenerateOverlapEvents(true);
	PoleColliderComponent->SetCapsuleRadius(10.0f);
	PoleColliderComponent->SetCapsuleHalfHeight(120.0f);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectBossCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Setup move forward & backward functions
	PlayerInputComponent->BindAxis("MoveForward", this, &AProjectBossCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AProjectBossCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AProjectBossCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AProjectBossCharacter::LookUpAtRate);

	// Listen to button release for the basic melee attack
	PlayerInputComponent->BindAction("BasicAttack", IE_Released, this, &AProjectBossCharacter::PerformMeleeAttack);
	PlayerInputComponent->BindAction("AdvancedAttack", IE_Released, this, &AProjectBossCharacter::PerformAdvancedAttack);
	// Listen for Ability OnReleased button press
	PlayerInputComponent->BindAction("AbilityOne", IE_Released, this, &AProjectBossCharacter::PerformAbilityOne);
	PlayerInputComponent->BindAction("AbilityTwo", IE_Released, this, &AProjectBossCharacter::PerformAbilityTwo);
}

void AProjectBossCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (PoleColliderComponent)
	{
		PoleColliderComponent->AttachTo(GetMesh(), "weapon_r", EAttachLocation::SnapToTarget, true);
		PoleColliderComponent->AddLocalRotation(FRotator(0, 0, 90.0f));
	}
}

void AProjectBossCharacter::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

	// Update Cooldown variables
	if (MeleeAtkCurrentCd >= 0)
		MeleeAtkCurrentCd -= deltaTime;
	if (AdvAttackCurrentCd >= 0)
		AdvAttackCurrentCd -= deltaTime;
	if (AbilOneCurrentCd >= 0)
		AbilOneCurrentCd -= deltaTime;

	if (m_offAbilOneIsFalling)
	{
		if (!GetMovementComponent()->IsFalling())
		{
			AbilityOneLandDamage();
		}
		else
		{
			// Play falling anim
			this->PlayAnimMontage(AbilityOneMontages[1]);
		}
	}
}

void AProjectBossCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AProjectBossCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AProjectBossCharacter::MoveForward(float Value)
{
	if (m_disableLocomotionMovement)
		return;
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AProjectBossCharacter::MoveRight(float Value)
{
	if (m_disableLocomotionMovement)
		return;
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

EStance AProjectBossCharacter::GetStance()
{
	return CurrentStance;
}

void AProjectBossCharacter::PerformMeleeAttack()
{
	if (AttackAnimMontages.Num() <= 0)
		return;

	if (m_isPerformingAbility)
		return;

	if (m_isAttacking)
	{
		m_saveAttack = true;
		m_hasAttackedThisSwing = false;
	}
	else
	{
		m_isAttacking = true;
		MeleeAtkCurrentCd = MeleeAttackCooldown;
		UE_LOG(LogTemp, Log, TEXT("Player performs Melee Attack"));

		this->PlayAnimMontage(AttackAnimMontages[m_attackCount], 0.75f);

		m_attackCount++;
		if (m_attackCount >= AttackAnimMontages.Num())
		{
			m_attackCount = 0;
		}
	}
}

void AProjectBossCharacter::ComboAttackSave()
{
	if (AttackAnimMontages.Num() <= 0)
		return;

	if (m_saveAttack)
	{
		// Check montage is playing before confirming
		float playDuration = this->PlayAnimMontage(AttackAnimMontages[m_attackCount], 0.75f);
		if (playDuration > 0.0f)
		{
			MeleeAtkCurrentCd = MeleeAttackCooldown;
			m_saveAttack = false;
			m_attackCount++;
			if (m_attackCount >= AttackAnimMontages.Num())
				m_attackCount = 0;
		}
	}
}

void AProjectBossCharacter::ResetCombo()
{
	m_isAttacking = false;
	m_saveAttack = false;
	m_attackCount = 0;
	m_hasAttackedThisSwing = false;
}

// RMB Advanced Attack
void AProjectBossCharacter::PerformAdvancedAttack()
{
	if (m_isPerformingAbility)
		return;

	if (CurrentStance == EStance::Offensive)
	{
		// Push staff at enemy.
		if (!AdvancedAttackMontage)
		{
			UE_LOG(LogTemp, Error, TEXT("No Montage set for AdvancedAttack"));
			return;
		}

		if (AdvAttackCurrentCd <= 0 && !GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogTemp, Log, TEXT("Using Offensive advanced ability"));

			AdvAttackCurrentCd = AdvAttackOffensiveTotalCooldown;
			float playDuration = this->PlayAnimMontage(AdvancedAttackMontage);
			if (playDuration <= 0.0f)
				UE_LOG(LogTemp, Error, TEXT("Unable to play AdvancedAttackMontage"));
		}
	}
	else if (CurrentStance == EStance::Evasive)
	{
		// Push staff into ground and leap
		if (!AdvancedEvadeMontage)
		{
			UE_LOG(LogTemp, Error, TEXT("No montage set for AdvancedEvadeMonatge"));
			return;
		}

		if (AdvAttackCurrentCd <= 0 && !GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogTemp, Log, TEXT("Using Evasive advanced ability"));

			AdvAttackCurrentCd = AdvAttackOffensiveTotalCooldown;
			this->PlayAnimMontage(AdvancedEvadeMontage);

			LaunchCharacter(FVector(0, 0, 900.0f), false, true);
		}
	}
}

void AProjectBossCharacter::AdvancedAttackLandDamage()
{
	// Spawn AOE damage collider and configure
	ACapsuleAOEDamage* dmgCollider = GetWorld()->SpawnActor<ACapsuleAOEDamage>(ACapsuleAOEDamage::StaticClass(), FActorSpawnParameters());
	float aoeHalfHeight = 250.0f;
	dmgCollider->ConfigureDamage(AdvAttackOffensiveDamageAmount, GetController(), this);
	dmgCollider->ConfigureCapsule(50.0f, aoeHalfHeight);
	dmgCollider->SetLifeSpan(0.25f);

	FVector capsuleSpawnLoc = GetActorLocation() + (GetActorForwardVector() * aoeHalfHeight);
	FRotator capsuleSpawnRot = FRotator(90.0f, GetActorRotation().Yaw, 0);
	dmgCollider->SetActorLocation(capsuleSpawnLoc);
	dmgCollider->SetActorRotation(capsuleSpawnRot);

	// Draw additional debug capsule for debug
	DrawDebugCapsule(GetWorld(), capsuleSpawnLoc, aoeHalfHeight, 50.0f, capsuleSpawnRot.Quaternion(), FColor::Green, false, 1.0f, 0, 2.0f);
}

void AProjectBossCharacter::PerformAbilityOne()
{
	if (m_isPerformingAbility)
		return;

	// Start of AbilityOne
	if (CurrentStance == EStance::Offensive)
	{
		// Thrust pole forward for damage and stun
		if (AbilityOneMontages.Num() <= 0)
		{
			UE_LOG(LogTemp, Error, TEXT("No AbilityOneMontage has been set!"));
			return;
		}

		// If not on cooldown & is in air
		if (AbilOneCurrentCd <= 0 && GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogTemp, Log, TEXT("Using AbilityOne Offensive"));

			// Set ability on cooldown
			AbilOneCurrentCd = AbilityOneTotalCooldown;
			m_disableLocomotionMovement = true;

			// Play montage and check it's playing
			float playDuration = this->PlayAnimMontage(AbilityOneMontages[0]);
			if (playDuration <= 0.0f)
				UE_LOG(LogTemp, Error, TEXT("Unable to play AbilityOneMontage"));

			// Apply additional double jump
			UCharacterMovementComponent* movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
			if (movement)
				LaunchCharacter(FVector(0, 0, movement->JumpZVelocity), false, true);

			m_isPerformingAbility = true;
		}
	}
	else if (CurrentStance == EStance::Evasive)
	{
		// Evasive cloudwalk ability
		if (!AbilityOneEvasiveMontage)
		{
			UE_LOG(LogTemp, Error, TEXT("AbilityOneEvasiveMontage not set!"));
			return;
		}

		if (AbilOneCurrentCd <= 0 && GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogTemp, Log, TEXT("Using AbilityOne Evasive Cloudwalk"));

			AbilOneCurrentCd = AbilityOneTotalCooldown;

			// Apply additional double jump
			UCharacterMovementComponent* movement = Cast<UCharacterMovementComponent>(GetMovementComponent());
			if (movement)
				LaunchCharacter(FVector(0, 0, movement->JumpZVelocity), false, true);

			// Play double jump montage
			this->PlayAnimMontage(AbilityOneEvasiveMontage);

			m_isPerformingAbility = true;
		}
	}
}

void AProjectBossCharacter::AbilityOneForceGround()
{
	// Stop any X/Y movement and slam down with force
	GetCharacterMovement()->Velocity += FVector(0, 0, -2500.0f);

	this->PlayAnimMontage(AbilityOneMontages[1]);
	m_offAbilOneIsFalling = true;
}

void AProjectBossCharacter::AbilityOneLandDamage()
{
	m_offAbilOneIsFalling = false;

	// Once slam has landed on ground
	this->PlayAnimMontage(AbilityOneMontages[2]);

	FVector colliderLocation = this->GetActorLocation() + FVector(0, 0, -50.0f);

	ACapsuleAOEDamage* aoeCapsuleCollider = GetWorld()->SpawnActor<ACapsuleAOEDamage>(ACapsuleAOEDamage::StaticClass(), FActorSpawnParameters());
	aoeCapsuleCollider->ConfigureDamage(AbilityOneDamageAmount, GetController(), this);
	aoeCapsuleCollider->ConfigureCapsule(AbilityOneRadius, AbilityOneRadius);
	aoeCapsuleCollider->SetActorLocation(colliderLocation);
	aoeCapsuleCollider->SetLifeSpan(0.25f);

	// Draw additional debug capsule for debug
	DrawDebugSphere(GetWorld(), colliderLocation, AbilityOneRadius, 20.0f, FColor::Green, false, 1.0f, 0, 2.0f);
}

void AProjectBossCharacter::FinishAbilityOne()
{
	// Once ability has finished animation
	UE_LOG(LogTemp, Log, TEXT("Finished AbilityOne"));
	m_disableLocomotionMovement = false;
	m_isPerformingAbility = false;
}

void AProjectBossCharacter::AbilityOneEvasiveCloudwalk()
{
	// Walk in the air for 1 second

	// Create walkable cloud below player's feet to enable "cloudwalking"
	AActor* cloud = GetWorld()->SpawnActor<ACloudwalkCloud>(ACloudwalkCloud::StaticClass(), FActorSpawnParameters());
	cloud->SetActorLocation(GetActorLocation() + FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	Cast<ACloudwalkCloud>(cloud)->SetTrackingArray(&m_spawnedClouds);
	m_spawnedClouds.Add(cloud);

	float cloudDurationSecs = 1.0f;
	GetWorldTimerManager().SetTimer(m_cloudwalkDelayTimer, this, &AProjectBossCharacter::CloudwalkDisable, cloudDurationSecs, false);

	m_isPerformingAbility = true;
}

void AProjectBossCharacter::CloudwalkDisable()
{
	UE_LOG(LogTemp, Log, TEXT("Disabling Evasive Cloudwalk"));

	if (m_spawnedClouds.Num() > 0)
	{
		for (int i = 0; i < m_spawnedClouds.Num(); i++)
		{
			GetWorld()->DestroyActor(m_spawnedClouds[i]);
		}
	}

	m_isPerformingAbility = false;
}

void AProjectBossCharacter::PerformAbilityTwo()
{
	// Cant change stance when performing abilities
	if (m_isPerformingAbility)
		return;

	switch (CurrentStance)
	{
		case EStance::Offensive:
			CurrentStance = EStance::Evasive;
			UE_LOG(LogTemp, Log, TEXT("Current stance is Evasive"));
			break;
		case EStance::Evasive:
			CurrentStance = EStance::Offensive;
			UE_LOG(LogTemp, Log, TEXT("Current stance is Offensive"));
			break;
		default:
			UE_LOG(LogTemp, Error, TEXT("Unable to switch stance, current unknown stance"));
			break;
	}
	
}

float AProjectBossCharacter::GetCurrentHealth()
{
	return CurrentHealth;
}

float AProjectBossCharacter::GetTotalHealth()
{
	return TotalHealth;
}

void AProjectBossCharacter::OnPoleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ABossCharacter::StaticClass()) && !m_hasAttackedThisSwing)
	{
		//UE_LOG(LogTemp, Log, TEXT("Pole collided with '%s'"), *OtherActor->GetName());
		// Apply damage if pole collision happens during attacking
		if (m_isAttacking && !m_hasAttackedThisSwing)
		{
			m_hasAttackedThisSwing = true;

			FDamageEvent dmgEvent;
			OtherActor->TakeDamage(MeleeAttackDamageAmount, dmgEvent, GetController(), this);
		}
	}
}

void AProjectBossCharacter::OnPoleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ABossCharacter::StaticClass()))
	{
		//UE_LOG(LogTemp, Log, TEXT("Pole finished colliding with '%s'"), *OtherActor->GetName());
	}
}

void AProjectBossCharacter::OnDeath()
{
	UE_LOG(LogTemp, Log, TEXT("Player has died!"));

	if (OnCharacterDied.IsBound())
		OnCharacterDied.Broadcast();

	// Ragdoll character on it's death
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

float AProjectBossCharacter::TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser)
{
	float damage = Super::TakeDamage(damageAmount, damageEvent, eventInstigator, damageCauser);

	CurrentHealth -= damage;

	UE_LOG(LogTemp, Log, TEXT("Player Health: %f"), CurrentHealth);
	if (CurrentHealth <= 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Player has died!"));
		CurrentHealth = 0;

		OnDeath();
	}

	return damage;
}
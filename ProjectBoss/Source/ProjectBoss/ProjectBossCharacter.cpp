// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectBossCharacter.h"
#include "ProjectBoss.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Boss\BossCharacter.h"
#include "Components/SphereComponent.h"
#include "Player\CapsuleAOEDamage.h"
#include "Player/CloudwalkCloud.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "UI/BossFightHUD.h"
#include "Statistics\CombatStats.h"

//////////////////////////////////////////////////////////////////////////
// AProjectBossCharacter

AProjectBossCharacter::AProjectBossCharacter()
{
	TotalHealth = 2000.0f;
	CurrentHealth = TotalHealth;

	AdvAttackCurrentCd = 0.0f;
	AdvAttackOffensiveTotalCooldown = 10.0f;
	AdvAttackOffensiveDamageAmount = 250.0f;

	AdvAttackEvasiveCloudDuration = 2.0f;
	
	AbilityOneTotalCooldown = 10.0f;
	AbilityOneRadius = 500.0f;
	AbilOneStunDuration = 1.5f;
	AbilOneCurrentCd = 0.0f;
	AbilityOneDamageAmount = 100.0f;

	AbilityTwoTotalCooldown = 1.0f;
	AbilTwoCurrentCd = 0.0f;
	Evasive_AttackRate = 0.75f;
	Evasive_MaxMS = 700.0f;
	Evasive_MeleeDamageAmount = 30.0f;
	Offensive_MeleeDamageAmount = 50.0f;
	Offensive_AttackRate = 1.25f;
	Offensive_MaxMS = 600.0f;

	m_hasAttackedThisSwing = false;

	CurrentStance = EStance::Offensive;
	m_attackRate = 0.95f;
	MeleeAttackCooldown = m_attackRate;

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

	PoleColliderComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	PoleColliderComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	PoleColliderComponent->SetGenerateOverlapEvents(true);
	PoleColliderComponent->SetCapsuleRadius(10.0f);
	PoleColliderComponent->SetCapsuleHalfHeight(120.0f);

	PS_PoleStance = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("PoleStancePaticleSystem"));
	PS_PoleStance->SetupAttachment(PoleColliderComponent);

	m_audioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	m_audioComponent->bAutoActivate = false;
	m_audioComponent->SetupAttachment(RootComponent);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectBossCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AProjectBossCharacter::OnJump);
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

	// Cast the movement component to the character movemenet component
	m_charMovementComponent = Cast<UCharacterMovementComponent>(GetMovementComponent());

	// Init combat stats class
	m_combatStats = NewObject<UCombatStats>();

	// Configure character to use Mouse rotation
	m_charMovementComponent->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = true;

	// Init pole arm collider used to track melee damage
	if (PoleColliderComponent)
	{
		// Attach pole collider to weapon_r bone on mesh
		FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, true);
		PoleColliderComponent->AttachToComponent(GetMesh(), rules, "weapon_r");
		PoleColliderComponent->AddLocalRotation(FRotator(0, 0, 90.0f));

		PS_PoleStance->AddLocalRotation(FRotator(90.0f, 0, 0));
		PS_PoleStance->SetTemplate(OffensivePolePS);
	}

	// Set initial stance to offensive
	SetStance(EStance::Offensive);
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
	if (AbilTwoCurrentCd >= 0)
		AbilTwoCurrentCd -= deltaTime;

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

void AProjectBossCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Log out end statistics for the character
	FString statsStr = m_combatStats->GetAllStatsString();
	UE_LOG(LogBoss, Log, TEXT("---\nPlayer End Play Statistics:\n%s\n---"), *statsStr);
	// Print character's final health
	UE_LOG(LogBoss, Log, TEXT("Player Health (Current/Total): %f/%f"), GetCurrentHealth(), GetTotalHealth());
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
	// Check we have attack montages to play
	if (AttackAnimMontages.Num() <= 0)
	{
		UE_LOG(LogPlayer, Error, TEXT("Perform Melee: No Attack Montages set on player!"));
		return;
	}

	// Dont continue if performing another ability
	if (m_isPerformingAbility)
	{
		return;
	}

	// If save attack flag has already been set before melee animation has reached ComboAttackSave called
	if (m_saveAttack)
	{
		// Check attack montage is still being played and exit.
		UAnimMontage* currentMontage = GetCurrentMontage();
		if (IsValid(currentMontage))
		{
			return;
		}
		else
		{
			// Save attack flag has been set but no attack animation in progress, reset melee flags
			UE_LOG(LogPlayer, Error, TEXT("Trying to melee but m_saveAttack is true with no melee montage! Resetting Melee state"));
			m_isAttacking = false;
			m_saveAttack = false;
		}
	}

	// If Attack montage is already in progress
	if (m_isAttacking)
	{
		// Check Attack montage is still in progress
		UAnimMontage* currentMontage = GetCurrentMontage();
		if (IsValid(currentMontage))
		{
			// Set to save attack
			m_saveAttack = true;
			m_hasAttackedThisSwing = false;
		}
		else
		{
			// No attack montage is being played, reset melee flags
			UE_LOG(LogPlayer, Error, TEXT("Tried to save attack but no montage playing!"));
			m_isAttacking = false;
		}
	}
	else
	{
		// Set is attacking and cooldown for melee
		m_isAttacking = true;

		UE_LOG(LogPlayer, Log, TEXT("Player performs Melee Attack"));

		// perform melee
		DoMelee();
	}
}

void AProjectBossCharacter::ComboAttackSave()
{
	// Called by every attack montage at point of save attack

	// Check attack montages exist
	if (AttackAnimMontages.Num() <= 0)
	{
		UE_LOG(LogPlayer, Error, TEXT("ComboAttackSave: No Attack Montages set on player!"));
		return;
	}

	// Only continue if right click press again (save attack flag)
	if (m_saveAttack)
	{
		// Disable save attack flag
		m_saveAttack = false;

		UE_LOG(LogPlayer, Log, TEXT("Player performs Combo Melee Attack"));

		// Perform melee
		DoMelee();
	}
}

void AProjectBossCharacter::DoMelee()
{
	if (AttackAnimMontages.Num() <= 0)
	{
		UE_LOG(LogPlayer, Error, TEXT("DoMelee: No Attack Montages set on player!"));
		return;
	}

	// Set cooldown of melee
	// SAVE_ATTACK_TIME = time in seconds of save attack mark in Montage
	MeleeAtkCurrentCd = SAVE_ATTACK_TIME * m_attackRate;

	// Play attack montage on actor
	this->PlayAnimMontage(AttackAnimMontages[m_attackCount], m_attackRate);

	// Add attempted attack to stats
	m_combatStats->AddAttack();

	// Increment attack count and wrap number back to 0 if reached end
	m_attackCount++;
	if (m_attackCount >= AttackAnimMontages.Num())
	{
		m_attackCount = 0;
	}
}


void AProjectBossCharacter::ResetCombo()
{
	// Reset all flags
	m_isAttacking = false;
	m_saveAttack = false;
	m_attackCount = 0;
	m_hasAttackedThisSwing = false;
}

// RMB Advanced Attack
void AProjectBossCharacter::PerformAdvancedAttack()
{
	if (m_isPerformingAbility)
	{
		return;
	}

	// If advanced attack on cooldown, play voice cue
	if (AdvAttackCurrentCd > 0)
	{
		PlayCue(CooldownVoiceCue);
		return;
	}

	if (CurrentStance == EStance::Offensive)
	{
		/*
		* Advanced Ability: Offensive
		* Shoot fire out of end of staff, forward
		*/
		if (!AdvancedAttackMontage)
		{
			UE_LOG(LogPlayer, Error, TEXT("No Montage set for AdvancedAttack"));
			return;
		}

		// Check ability isn't on cooldown and isn't falling
		if (AdvAttackCurrentCd <= 0 && !GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogPlayer, Log, TEXT("Using Offensive advanced ability"));

			// Add offensive advanced ability attempt
			m_combatStats->AddAbilityAttempt(EPlayerAbilities::Offensive_AdvAbility);

			// Set advanced ability on cooldown and play montage
			AdvAttackCurrentCd = AdvAttackOffensiveTotalCooldown;
			float playDuration = this->PlayAnimMontage(AdvancedAttackMontage);
			if (playDuration <= 0.0f)
				UE_LOG(LogPlayer, Error, TEXT("Unable to play AdvancedAttackMontage"));

			// Set is performing flag
			m_isPerformingAbility = true;
		}
	}
	else if (CurrentStance == EStance::Evasive)
	{
		/* 
		* Advanced Ability: Evasive
		* Push staff into ground and leap
		*/
		if (!AdvancedEvadeMontage)
		{
			UE_LOG(LogPlayer, Error, TEXT("No montage set for AdvancedEvadeMonatge"));
			return;
		}

		// Check we aren't falling and isn't on cooldown
		if (AdvAttackCurrentCd <= 0 && !GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogPlayer, Log, TEXT("Using Evasive advanced ability"));

			// Add evasive advanced ability attempt and success. Will always be success since no dmg is dealt
			m_combatStats->AddAbilityAttempt(EPlayerAbilities::Evasive_AdvAbility);
			m_combatStats->AddAbilitySuccess(EPlayerAbilities::Evasive_AdvAbility);

			// Set ability on cooldown and play montage
			AdvAttackCurrentCd = AdvAttackOffensiveTotalCooldown;
			this->PlayAnimMontage(AdvancedEvadeMontage);

			// Add vertical force to character to launch in air
			LaunchCharacter(FVector(0, 0, 900.0f), false, true);

			// Play Evasive advanced ability cue
			if (IsValid(Evasive_AdvAbilSoundCue))
			{
				PlayCue(Evasive_AdvAbilSoundCue, true, 1.0f, FMath::RandRange(0.8f, 1.2f));
			}
		}
	}
}

void AProjectBossCharacter::AdvancedAttackLandDamage()
{
	/* 
	* Called by AnimMontage, once fire appears out of staff in montage
	*/

	// Play offensive sound cue
	if (IsValid(Offensive_AdvAbilSoundCue))
	{
		PlayCue(Offensive_AdvAbilSoundCue, true, 0.2f);
	}

	// Spawn AOE damage collider and configure
	ACapsuleAOEDamage* dmgCollider = GetWorld()->SpawnActor<ACapsuleAOEDamage>(ACapsuleAOEDamage::StaticClass(), FActorSpawnParameters());
	// Listen to event when capsule deals damage
	dmgCollider->OnCapsuleDealtDamage.AddDynamic(this, &AProjectBossCharacter::AdvAbilityDealtDamage);

	// Set damage and size dimensions of capsule
	float aoeHalfHeight = 250.0f;
	dmgCollider->ConfigureDamage(AdvAttackOffensiveDamageAmount, GetController(), this);
	dmgCollider->ConfigureCapsule(50.0f, aoeHalfHeight);
	dmgCollider->SetLifeSpan(0.25f);

	// Set spawn location and rotation. Rotation is spawning in front of actor
	FVector capsuleSpawnLoc = GetActorLocation() + (GetActorForwardVector() * aoeHalfHeight);
	FRotator capsuleSpawnRot = FRotator(90.0f, GetActorRotation().Yaw, 0);
	dmgCollider->SetActorLocation(capsuleSpawnLoc);
	dmgCollider->SetActorRotation(capsuleSpawnRot);

	// Draw additional debug capsule for debug
	//DrawDebugCapsule(GetWorld(), capsuleSpawnLoc, aoeHalfHeight, 50.0f, capsuleSpawnRot.Quaternion(), FColor::Green, false, 1.0f, 0, 2.0f);

	// Reset performing flag
	m_isPerformingAbility = false;
}

void AProjectBossCharacter::PerformAbilityOne()
{
	if (m_isPerformingAbility)
	{
		// Performing this/another ability
		return;
	}

	if (AbilOneCurrentCd > 0)
	{
		// Ability on COoldown
		PlayCue(CooldownVoiceCue, false);
		return;
	}

	// Start of AbilityOne
	if (CurrentStance == EStance::Offensive)
	{
		/*
		* Ability One: Offensive
		* Leap forward in the air and launch down to ground
		*/
		if (AbilityOneMontages.Num() <= 0)
		{
			UE_LOG(LogPlayer, Error, TEXT("No AbilityOneMontage has been set!"));
			return;
		}

		// If not on cooldown & *is* in air
		if (AbilOneCurrentCd <= 0 && GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogPlayer, Log, TEXT("Using AbilityOne Offensive"));

			// Add evasive ability one attempt
			m_combatStats->AddAbilityAttempt(EPlayerAbilities::Offensive_AbilityOne);

			// Set ability on cooldown and set flags
			AbilOneCurrentCd = AbilityOneTotalCooldown;
			m_disableLocomotionMovement = true;

			// Play montage and check it's playing
			float playDuration = this->PlayAnimMontage(AbilityOneMontages[0]);
			if (playDuration <= 0.0f)
				UE_LOG(LogPlayer, Error, TEXT("Unable to play AbilityOneMontage"));

			// Play jump grunt with rnd pitch
			if (IsValid(JumpGruntSoundCue))
			{
				PlayCue(JumpGruntSoundCue, true, 1.0f, FMath::RandRange(0.8f, 1.2f));
			}

			// Apply additional double jump
			if (m_charMovementComponent)
				LaunchCharacter(FVector(0, 0, m_charMovementComponent->JumpZVelocity), false, true);
		}
	}
	else if (CurrentStance == EStance::Evasive)
	{
		/*
		* Ability One: Evasive
		* Leap into air and walk on clouds
		*/
		if (!AbilityOneEvasiveMontage)
		{
			UE_LOG(LogPlayer, Error, TEXT("AbilityOneEvasiveMontage not set!"));
			return;
		}

		// Check Ability One isnt in cooldown and *is* in air
		if (AbilOneCurrentCd <= 0 && GetMovementComponent()->IsFalling())
		{
			UE_LOG(LogPlayer, Log, TEXT("Using AbilityOne Evasive Cloudwalk"));

			// Add evasive ability one attempt
			m_combatStats->AddAbilityAttempt(EPlayerAbilities::Evasive_AbilityOne);

			// Put ability one on cooldown
			AbilOneCurrentCd = AbilityOneTotalCooldown;

			// Play jump grunt with rnd pitch
			if (IsValid(JumpGruntSoundCue))
			{
				PlayCue(JumpGruntSoundCue, true, 1.0f, FMath::RandRange(0.8f, 1.2f));
			}

			// Play double jump montage
			this->PlayAnimMontage(AbilityOneEvasiveMontage);

			// Apply additional double jump
			if (m_charMovementComponent)
			{
				LaunchCharacter(FVector(0, 0, m_charMovementComponent->JumpZVelocity), false, true);
			}
		}
	}

	m_isPerformingAbility = true;
}

void AProjectBossCharacter::AbilityOneForceGround()
{
	// Stop any X/Y movement and slam down with force
	GetCharacterMovement()->Velocity += FVector(0, 0, -2500.0f);

	// Play next montage in sequence
	if (AbilityOneMontages.Num() > 1)
	{
		this->PlayAnimMontage(AbilityOneMontages[1]);
	}
	m_offAbilOneIsFalling = true;
}

void AProjectBossCharacter::AbilityOneLandDamage()
{
	// Once slam has landed on ground

	// Play slam montage
	this->PlayAnimMontage(AbilityOneMontages[2]);

	// Play sound cue for explosion if exists
	if (IsValid(Offensive_AbilityOneSoundCue))
	{
		PlayCue(Offensive_AbilityOneSoundCue, true, 0.2f);
	}

	// Determine floor location to create explosion
	FVector colliderLocation = this->GetActorLocation() + FVector(0, 0, -50.0f);

	// Spawn capsule and listen to capsule event on dealing damage
	ACapsuleAOEDamage* aoeCapsuleCollider = GetWorld()->SpawnActor<ACapsuleAOEDamage>(ACapsuleAOEDamage::StaticClass(), colliderLocation, FRotator(), FActorSpawnParameters());
	aoeCapsuleCollider->OnCapsuleDealtDamage.AddDynamic(this, &AProjectBossCharacter::AbilOneDealtDamage);

	// Configure capsule for Ability One
	aoeCapsuleCollider->ConfigureDamage(AbilityOneDamageAmount, GetController(), this);
	aoeCapsuleCollider->SetStunDuration(AbilOneStunDuration);
	aoeCapsuleCollider->ConfigureCapsule(AbilityOneRadius, AbilityOneRadius);
	aoeCapsuleCollider->SetLifeSpan(0.25f);

	// Draw additional debug capsule for debug
	//DrawDebugSphere(GetWorld(), colliderLocation, AbilityOneRadius, 20.0f, FColor::Green, false, 1.0f, 0, 2.0f);

	// Disable ability one falling flag
	m_offAbilOneIsFalling = false;
}

void AProjectBossCharacter::FinishAbilityOne()
{
	// Once ability has finished animation
	UE_LOG(LogPlayer, Log, TEXT("Finished AbilityOne"));
	m_disableLocomotionMovement = false;
	m_isPerformingAbility = false;
}

bool AProjectBossCharacter::IsEvading()
{
	return GetMovementComponent()->IsFalling() || m_spawnedClouds.Num() > 0;
}

int AProjectBossCharacter::GetAbilityAttempts(EPlayerAbilities ability)
{
	return m_combatStats->GetAbilityAttempts(ability);
}

int AProjectBossCharacter::GetAbilitySuccesses(EPlayerAbilities ability)
{
	return m_combatStats->GetAbilitySuccessfulAttempts(ability);
}

UCombatStats* AProjectBossCharacter::GetCombatStats()
{
	return m_combatStats;
}

void AProjectBossCharacter::AbilityOneEvasiveCloudwalk()
{
	/*
	*  Walk in the air for 1 second
	*/

	// Create walkable cloud below player's feet to enable "cloudwalking"
	AActor* cloud = GetWorld()->SpawnActor<ACloudwalkCloud>(ACloudwalkCloud::StaticClass(), FActorSpawnParameters());
	cloud->SetActorLocation(GetActorLocation() + FVector(0, 0, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	Cast<ACloudwalkCloud>(cloud)->SetTrackingArray(&m_spawnedClouds);
	m_spawnedClouds.Add(cloud);

	// Start timer to destroy clouds after delay
	GetWorldTimerManager().SetTimer(m_cloudwalkDelayTimer, this, &AProjectBossCharacter::CloudwalkDisable, AdvAttackEvasiveCloudDuration, false);

	// Set performing flag
	m_isPerformingAbility = true;
}

void AProjectBossCharacter::CloudwalkDisable()
{
	UE_LOG(LogPlayer, Log, TEXT("Disabling Evasive Cloudwalk"));

	// If spawned clouds contains any created clouds
	if (m_spawnedClouds.Num() > 0)
	{
		// Destroy all clouds (actors)
		for (int i = 0; i < m_spawnedClouds.Num(); i++)
		{
			GetWorld()->DestroyActor(m_spawnedClouds[i]);
		}
	}

	// Evasive ability success end once all clouds destroyed
	// Should always success since evasive deals no dmg
	m_combatStats->AddAbilitySuccess(EPlayerAbilities::Evasive_AbilityOne);

	// Set performing flag
	m_isPerformingAbility = false;
}

void AProjectBossCharacter::PerformAbilityTwo()
{
	// Cant change stance when performing abilities
	if (m_isPerformingAbility)
	{
		return;
	}
	if (AbilTwoCurrentCd > 0)
	{
		PlayCue(CooldownVoiceCue);
		return;
	}

	switch (CurrentStance)
	{
		case EStance::Offensive:
			SetStance(EStance::Evasive);
			break;
		case EStance::Evasive:
			SetStance(EStance::Offensive);
			break;
		default:
			UE_LOG(LogPlayer, Error, TEXT("Unable to switch stance, current unknown stance"));
			break;
	}

	AbilTwoCurrentCd = AbilityTwoTotalCooldown;
}

void AProjectBossCharacter::SetStance(EStance targetStance)
{
	CurrentStance = targetStance;

	UE_LOG(LogPlayer, Log, TEXT("Changed stance to '%s'"), *StanceToString(CurrentStance));

	if (CurrentStance == EStance::Evasive)
	{
		m_attackRate = Evasive_AttackRate;
		m_charMovementComponent->MaxWalkSpeed = Evasive_MaxMS;

		// Set pole PS to evasive
		if (EvasivePolePS) {
			PS_PoleStance->SetTemplate(EvasivePolePS);
		}
	}
	else if (CurrentStance == EStance::Offensive)
	{
		m_attackRate = Offensive_AttackRate;
		m_charMovementComponent->MaxWalkSpeed = Offensive_MaxMS;

		// Set Pole Stance particle system to Offensive
		if (OffensivePolePS) {
			PS_PoleStance->SetTemplate(OffensivePolePS);
		}
	}

	UE_LOG(LogPlayer, Log, TEXT("Attack Rate: '%f', Max Walk Speed: '%f'"), m_attackRate, m_charMovementComponent->MaxWalkSpeed);
	// Update attack rate cooldown
	MeleeAttackCooldown = m_attackRate;
}

bool AProjectBossCharacter::PlayCue(USoundBase* sound, bool shouldOverrideExistingSound, float volumeMultiplier, float pitchMultiplier)
{
	if (m_audioComponent)
	{
		// Check if component is playing a sound already
		if (m_audioComponent->IsPlaying())
		{
			if (shouldOverrideExistingSound)
			{
				// Stop the previous sound if shouldOverrideExistingSound
				m_audioComponent->Stop();
			}
			else
			{
				// Exit if other sound should be allowed to play
				return false;
			}
		}

		// Set sound and prepare component for this sound
		m_audioComponent->SetSound(sound);
		m_audioComponent->SetVolumeMultiplier(volumeMultiplier);
		m_audioComponent->SetPitchMultiplier(pitchMultiplier);

		// Play and return true
		m_audioComponent->Play();

		return true;
	}

	// Unable to play the sound
	return false;
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
		//UE_LOG(LogPlayer, Log, TEXT("Pole collided with '%s'"), *OtherActor->GetName());
		// Apply damage if pole collision happens during attacking
		if (m_isAttacking && !m_hasAttackedThisSwing)
		{
			m_hasAttackedThisSwing = true;

			// Determine correct damage from the current stance
			float stanceMeleeDmg = 0.0f;
			switch (CurrentStance)
			{
				case EStance::Offensive:
					stanceMeleeDmg = Offensive_MeleeDamageAmount;
					break;
				case EStance::Evasive:
					stanceMeleeDmg = Evasive_MeleeDamageAmount;
					break;
			}

			UE_LOG(LogPlayer, Log, TEXT("Player deals '%f' damage in stance '%s'"), stanceMeleeDmg, *StanceToString(CurrentStance));

			// Apply damage to the attacking target
			FDamageEvent dmgEvent;
			OtherActor->TakeDamage(stanceMeleeDmg, dmgEvent, GetController(), this);

			// Add amount to damage dealt
			m_combatStats->AddDamageDealt(stanceMeleeDmg);

			// Add hit marker to UI
			HUDAddHitMarker();

			// Add successful attack to stats
			m_combatStats->AddSuccessfulAttack();

			// Play melee impact sound
			if (AttackImpactSounds.Num() > 0)
			{
				// Random impact sound and random pitch
				int rndIndex = FMath::RandRange(0, AttackImpactSounds.Num() - 1);
				float rndPitch = FMath::RandRange(0.75f, 1.35f);
				PlayCue(AttackImpactSounds[rndIndex], true, 0.1f, rndPitch);
			}
		}
	}
}

void AProjectBossCharacter::OnPoleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(ABossCharacter::StaticClass()))
	{
		//UE_LOG(LogPlayer, Log, TEXT("Pole finished colliding with '%s'"), *OtherActor->GetName());
	}
}

void AProjectBossCharacter::OnDeath()
{
	UE_LOG(LogPlayer, Log, TEXT("Player has died!"));

	// Play death cue for character
	if (DeathSoundCue)
	{
		PlayCue(DeathSoundCue, true);
	}

	// Broadcast died event to any listeners
	if (OnCharacterDied.IsBound())
		OnCharacterDied.Broadcast();

	// Ragdoll character on it's death
	DetachFromControllerPendingDestroy();

	// Disable any collision on main capsule
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);

	SetActorEnableCollision(true);
	// Configure mesh to use ragdoll
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->WakeAllRigidBodies();
	GetMesh()->bBlendPhysics = true;

	// Configure movement component
	if (m_charMovementComponent)
	{
		m_charMovementComponent->StopMovementImmediately();
		m_charMovementComponent->DisableMovement();
		m_charMovementComponent->SetComponentTickEnabled(false);
	}

	// Disable pole capsule collision
	PoleColliderComponent->SetCollisionProfileName(TEXT("NoCollision"));
}

float AProjectBossCharacter::TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser)
{
	float damage = Super::TakeDamage(damageAmount, damageEvent, eventInstigator, damageCauser);
	
	// Reduce current health with damage amount
	CurrentHealth -= damage;

	// Adds dmg amount to stats
	m_combatStats->AddDamageRecieved(damage);

	UE_LOG(LogPlayer, Log, TEXT("Player takes '%f' damage from '%s' (Total Health: '%f')"), damageAmount, *damageCauser->GetName(), CurrentHealth);

	// If health is below 0, proceed with death
	if (CurrentHealth <= 0)
	{
		UE_LOG(LogPlayer, Log, TEXT("Player has died!"));
		CurrentHealth = 0;

		OnDeath();
	}

	return damage;
}

void AProjectBossCharacter::AdvAbilityDealtDamage()
{
	// Add hit marker
	HUDAddHitMarker();

	// Add success to combat stats
	m_combatStats->AddAbilitySuccess(EPlayerAbilities::Offensive_AdvAbility);
}

void AProjectBossCharacter::AbilOneDealtDamage()
{
	// Add hit marker
	HUDAddHitMarker();

	// Add success to combat stats
	m_combatStats->AddAbilitySuccess(EPlayerAbilities::Offensive_AbilityOne);
}

void AProjectBossCharacter::HUDAddHitMarker()
{
	// Get the player controller and HUD, cast to scripts
	APlayerController* player = Cast<APlayerController>(GetController());
	ABossFightHUD* hud = Cast<ABossFightHUD>(player->GetHUD());
	
	// Add hitmarker to screen
	hud->AddHitMarker();
}

FString AProjectBossCharacter::StanceToString(EStance stance)
{
	switch (stance)
	{
	case EStance::Offensive:
		return "Offensive";
	case EStance::Evasive:
		return "Evasive";
	default:
		return "Unknown";
	}
}

void AProjectBossCharacter::OnJump()
{
	// Check character can jump and isn't airborne
	if (IsValid(m_charMovementComponent) && m_charMovementComponent->CanAttemptJump() && !m_charMovementComponent->IsFalling())
	{
		// Play jump grunt sound cue
		if (IsValid(JumpGruntSoundCue))
		{
			PlayCue(JumpGruntSoundCue, true, 1.0f, FMath::RandRange(0.9f, 1.1f));
		}

		// Use ACharacter's Jump function
		Jump();
	}
}
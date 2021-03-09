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
#include "../Statistics/CombatStats.h"
#include "UObject/UObjectGlobals.h"
#include "GOAP/GOAPAIController.h"

// Sets default values
ABossCharacter::ABossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set AI controller class & auto possess
	AIControllerClass = AGOAPAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	m_isPerformingAnyAbility = false;
	// General
	TotalHealth = 2500.0f;
	// Melee Attack
	m_attackCount = 0;
	m_isAttacking = false;
	m_saveAttack = false;
	m_dmgThisAttack = false;
	m_attackRateDifficulty = 0.50f;
	MinAttackRate = 0.80f;
	MaxAttackRate = 1.2f;
	MinMeleeDamage = 45.0f;
	MaxMeleeDamage = 55.0f;
	MeleeRadius = 150.0f;
	// Ability Advanced
	AdvAbilityDamage = 50.0f;
	AdvAbilityTotalCooldown = 20.0f;
	AdvAbilityCurrentCd = 0.0f;
	AdvAbilityMaxRange = 1500.0f;
	// Ability One
	AbilOneCritAmount = 150.0f;
	AbilOneTotalCooldown = 10.0f;
	AdvAbilityCurrentCd = 0.0f;
	m_isInvisible = false;
	MeleeCritMultiplier = 1.35f;
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

	// Set mesh
	if (auto mesh = GetMesh())
	{
		// Set location and rotation for Kallari
		mesh->SetRelativeLocation(FVector(0, 0, -90));
		//mesh->SetRelativeRotation(FRotator(0, -90, 0));
	}

	LeftBladeCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("LeftBladeCollider"));
	LeftBladeCollider->SetupAttachment(GetMesh());
	LeftBladeCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	LeftBladeCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	LeftBladeCollider->SetGenerateOverlapEvents(true);
	LeftBladeCollider->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnBladeBeginOverlap);
	//LeftBladeCollider->SetHiddenInGame(false);

	RightBladeCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RightBladeCollider"));
	RightBladeCollider->SetupAttachment(GetMesh());
	RightBladeCollider->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	RightBladeCollider->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	RightBladeCollider->SetGenerateOverlapEvents(true);
	RightBladeCollider->OnComponentBeginOverlap.AddDynamic(this, &ABossCharacter::OnBladeBeginOverlap);
	//RightBladeCollider->SetHiddenInGame(false);

	m_bossAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	m_bossAudioComponent->bAutoActivate = false;
	m_bossAudioComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ABossCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Init combat stats class
	m_combatStats = NewObject<UCombatStats>();

	// Set actor name
	FString name = "Kallari";
	Rename(*name);
	// Set start health to total on start
	CurrentHealth = TotalHealth;

	FAttachmentTransformRules rules(EAttachmentRule::SnapToTarget, false);
	// Set left and right blade colliders
	if (LeftBladeCollider)
	{
		LeftBladeCollider->AttachToComponent(GetMesh(), rules, "sword_handle_l");
		LeftBladeCollider->SetCapsuleHalfHeight(40.0f);
		LeftBladeCollider->SetCapsuleRadius(10.0f);
		LeftBladeCollider->AddLocalRotation(FRotator(90.0f, 0, 0));
		LeftBladeCollider->AddLocalOffset(FVector(0, 0, -30.0f));
	}
	if (RightBladeCollider)
	{
		RightBladeCollider->AttachToComponent(GetMesh(), rules, "sword_handle_r");
		RightBladeCollider->SetCapsuleHalfHeight(40.0f);
		RightBladeCollider->SetCapsuleRadius(10.0f);
		RightBladeCollider->AddLocalRotation(FRotator(90.0f, 0, 0));
		RightBladeCollider->AddLocalOffset(FVector(0, 0, 30.0f));
	}

	//m_originalMeshMaterials = GetMesh()->GetMaterials();
	if (GetMesh()) 
	{
		if (GetMesh()->GetNumMaterials() > 0)
		{
			// Iterate through all materials and make dynamic instance
			for (int i = 0; i < GetMesh()->GetNumMaterials(); i++)
			{
				m_originalMeshMaterials.Add(GetMesh()->CreateAndSetMaterialInstanceDynamicFromMaterial(i, GetMesh()->GetMaterial(i)));
			}
		}

		if (IsValid(InvisibleMatInst))
		{
			// Create dynamic invis material
			m_invisMatInst = GetMesh()->CreateDynamicMaterialInstance(0, InvisibleMatInst);
		}
	}

	// If no controller has spawned on boss, create the default one
	if (!GetController())
	{
		SpawnDefaultController();
	}
}

void ABossCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Set AI controller once possessed
	m_aiController = Cast<AAIController>(NewController);
}

void ABossCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Log out end statistics for the character
	FString statsStr = m_combatStats->GetAllStatsString();
	UE_LOG(LogBoss, Log, TEXT("---\nBoss End Play Statistics:\n%s\n---"), *statsStr);
	// Print character's final health
	UE_LOG(LogBoss, Log, TEXT("Boss' Health (Current/Total): %f/%f"), GetCurrentHealth(), GetTotalHealth());

	if (m_combatStats)
	{
		delete m_combatStats;
	}
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
		// Advanced Ability: Look at player when aiming to throw RMB attack
		if (IsValid(m_rmbTarget) && IsValid(m_aiController))
		{
			m_aiController->MoveToActor(m_rmbTarget, AdvAbilityMaxRange);
			LookAtActor(m_rmbTarget->GetActorLocation());
		}

		// Ultimate: Face actor towards target jump actor
		if (m_ultiIsChanneling && IsValid(m_ultiTargetActor))
		{
			LookAtActor(m_ultiTargetActor->GetActorLocation());
		}

		// Perform heal relevant code
		DoHeal(deltaTime);
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

	// Minus the damage amount from health
	CurrentHealth -= damage;

	// Adds amount to recieved
	m_combatStats->AddDamageRecieved(damage);

	UE_LOG(LogBoss, Log, TEXT("Boss takes '%f' damage from '%s' (Total Health: '%f')"), damage, *damageCauser->GetName(), CurrentHealth);

	// If character is below 0, has died
	if (CurrentHealth <= 0)
	{
		UE_LOG(LogBoss, Log, TEXT("Boss has died!"));
		CurrentHealth = 0;

		// perform on death action
		OnDeath();
	}

	return damage;
}

void ABossCharacter::PerformMeleeAttack()
{
	// Can't perform if any ability in progress
	if (IsPerformingAbility())
		return;

	// Save attack if in progress or perform normal melee
	if (m_isAttacking)
	{
		m_saveAttack = true;
	}
	else
	{
		DoMelee();
	}
}

void ABossCharacter::ComboAttackSave()
{
	// Only combo if saveAttack has been set to true (called PerformMeleeAttack again)
	if (m_saveAttack)
	{
		m_saveAttack = false;
		m_dmgThisAttack = false;

		DoMelee();
	}
}

void ABossCharacter::DoMelee()
{
	m_isAttacking = true;

	// Play current attack montage with the current attack rate
	float currentAtkRate = GetCurrentDifficultyAttackRate();
#if PROJ_BOSS_SCREEN_DEBUG
	if (GEngine)
	{
		FString msg = "Attack Rate: " + FString::SanitizeFloat(currentAtkRate) + " Attack Difficulty: " + FString::SanitizeFloat(m_attackRateDifficulty);
		GEngine->AddOnScreenDebugMessage(5, 5.0, FColor::White, msg);
	}
#endif
	// Play attack anim with attack rate
	this->PlayAnimMontage(AttackAnimMontages[m_attackCount], currentAtkRate);

	// Increment attack count
	m_attackCount++;
	if (m_attackCount >= AttackAnimMontages.Num())
	{
		m_attackCount = 0;
	}

	// Add attack statistic
	if (IsValid(m_combatStats))
	{
		m_combatStats->AddAttack();
	}
}

void ABossCharacter::PerformAdvancedAttack(AActor* target)
{
	if (!target || m_isPerformingAnyAbility)
		return;

	UE_LOG(LogBoss, Log, TEXT("Boss performing Advanced Attack"));

	m_isPerformingAnyAbility = true;

	// Set aim target and play aim animation
	m_rmbTarget = target;
	this->PlayAnimMontage(AdvancedAttackMontages[0]);
}

void ABossCharacter::SetInvisible(bool isInvis)
{
	// Set is invisible bool
	m_isInvisible = isInvis;

	if (m_combatStats)
	{
		// Add attempt & success, will always be a success
		m_combatStats->AddAbilityAttempt(EAbilities::One);
	}

	bool succeeded = false;
	// If original materials dynamic materials exist (have been created)
	if (IsValid(GetMesh()) && m_originalMeshMaterials.Num() > 0)
	{
		// Loop over dynamic materials
		for (int i = 0; i < m_originalMeshMaterials.Num(); i++)
		{
			if (isInvis)
			{
				// Set to invisible dynamic material
				GetMesh()->SetMaterial(i, m_invisMatInst);
				succeeded = true;
			}
			else
			{
				// Set to original dynamic material
				if (IsValid(m_originalMeshMaterials[i]))
				{
					GetMesh()->SetMaterial(i, m_originalMeshMaterials[i]);
					succeeded = true;
				}
				else
				{
					UE_LOG(LogBoss, Error, TEXT("Material '%d' invalid!"), i);
				}
			}
		}
	}

	// Run success code if executed
	if (succeeded)
	{
		AbilitySucceessful(EAbilities::One);
	}
}

bool ABossCharacter::MoveToLocation(FVector location)
{
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
	// Waited random time, Play the throw animation
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
		// Cast actor to dagger and configure
		AThrowableDagger* dagger = Cast<AThrowableDagger>(daggerActor);
		dagger->SetDamage(AdvAbilityDamage);
		// Listen to when dagger overlaps with player
		dagger->OnDaggerDealtDamage.AddDynamic(this, &ABossCharacter::OnAdvAttackDealtDamage);

		// Set its spawn location and look direction
		dagger->SetActorLocation(spawnLoc);
		FRotator lookAt = UKismetMathLibrary::FindLookAtRotation(dagger->GetActorLocation(), m_rmbTarget->GetActorLocation());
		FRotator daggerRot = dagger->GetActorRotation();
		dagger->SetActorRotation(lookAt + FRotator(-90, 0, 0));
		// Set movement direction to fly toward player at speed
		float daggerSpeed = 3000.0f;
		dagger->SetMovementDirection(dagger->GetActorUpVector(), daggerSpeed);
	}

	AdvAttackOnFinish();
}

void ABossCharacter::AdvAttackOnFinish()
{
	m_rmbTarget = nullptr;
	m_isPerformingAnyAbility = false;

	// Add attempt to stats
	if (m_combatStats)
	{
		m_combatStats->AddAbilityAttempt(EAbilities::Advanced);
	}

	// On cooldown once ability finished performing
	AdvAbilityCurrentCd = AdvAbilityTotalCooldown;
}

// On (Advanced Ability) dagger collided with player
void ABossCharacter::OnAdvAttackDealtDamage()
{
	AbilitySucceessful(EAbilities::Advanced);
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
	m_combatStats->AddAbilityAttempt(EAbilities::Ultimate);
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
	m_combatStats->AddAbilityAttempt(EAbilities::Heal);
}

void ABossCharacter::DoHeal(float deltaTime)
{
	// Heal: Heal ability if time still remaining
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

			// Perform cleanup for heal
			OnFinishHeal();
		}
	}
}

void ABossCharacter::OnFinishHeal()
{
	UE_LOG(LogBoss, Log, TEXT("Finished Heal ability to '%f' health. Cooldown for '%f' seconds"), CurrentHealth, HealTotalCooldown);

	HealCurrentCooldown = HealTotalCooldown;
	m_healTimeRemaining = 0;
	
	m_isPerformingAnyAbility = false;

	AbilitySucceessful(EAbilities::Heal);
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
	FVector targetLocation = m_ultiTargetActor->GetActorLocation() - (directionVector * 100.0f);
	// Teleport actor to location
	this->SetActorLocation(targetLocation);

	// Only apply damage if player didn't "avoid"/evade it
	AProjectBossCharacter* player = Cast<AProjectBossCharacter>(m_ultiTargetActor);
	if (!player->IsEvading())
	{
		// Apply damage for successful ability
		m_ultiTargetActor->TakeDamage(UltimateDamage, FDamageEvent(), GetController(), this);

		// Add dmg amount to dealt damage
		m_combatStats->AddDamageDealt(UltimateDamage);

		AbilitySucceessful(EAbilities::Ultimate);
	}

	// Play ultimate montage to attack player
	if (AbilityUltimateMontages.Num() > 1)
	{
		this->PlayAnimMontage(AbilityUltimateMontages[1]);
	}

	UltimateOnFinish();
}

void ABossCharacter::UltimateOnFinish()
{
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

float ABossCharacter::GetAdvancedAbilityDamage()
{
	return AdvAbilityDamage;
}

float ABossCharacter::GetAbilityOneCooldown()
{
	return AbilOneCurrentCd;
}

void ABossCharacter::OnDeath()
{
	UE_LOG(LogBoss, Log, TEXT("BossCharacter has died!"));

	// Play death cue for character
	if (DeathCue)
	{
		PlayCue(DeathCue);
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

	// Broadcast Boss death event
	if (OnCharacterDied.IsBound())
		OnCharacterDied.Broadcast();
}

void ABossCharacter::OnBladeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AProjectBossCharacter::StaticClass()) && m_isAttacking && !m_dmgThisAttack)
	{
		// Get the damage for this melee attack
		float rangeMeleeDmg = FMath::RandRange(MinMeleeDamage, MaxMeleeDamage);
		float dmgAmount = rangeMeleeDmg;

		// If performing certain ability that changes attack damage, set to that
		if (MeleeCritMultiplier > 0)
		{
			dmgAmount = rangeMeleeDmg * (MeleeCritMultiplier / 100);
			MeleeCritMultiplier = 0;
		}

		// Apply damage to other actor
		FDamageEvent dmgEvent;
		OtherActor->TakeDamage(dmgAmount, dmgEvent, GetController(), this);
		// Set damage flag to only deal once on this swing
		m_dmgThisAttack = true;

		// Adds dmg amount to stats
		m_combatStats->AddDamageDealt(dmgAmount);

		// Broadcast event for successful melee
		if (OnMeleeSucceeded.IsBound())
		{
			OnMeleeSucceeded.Broadcast();
		}

		// Add successful attack to stats
		m_combatStats->AddSuccessfulAttack();

		// Play Impact sound
		if (AttackSoundCues.Num() > 0)
		{
			// Random sound and random pitch
			int rndIndex = FMath::RandRange(0, AttackSoundCues.Num() - 1);
			float pitch = FMath::RandRange(0.7f, 1.35f);
			PlayCue(AttackSoundCues[rndIndex], 0.07f, pitch);
		}
	}
}

void ABossCharacter::UpdateAbilityCooldowns(float deltaTime)
{
	// Update Advanced Abilitycooldown
	if (AdvAbilityCurrentCd > 0)
		AdvAbilityCurrentCd -= deltaTime;
	// Update Ability One cooldown
	if (AbilOneCurrentCd > 0)
		AbilOneCurrentCd -= deltaTime;
	// Update Ultimate cooldown
	if (UltimateCurrentCd > 0)
		UltimateCurrentCd -= deltaTime;
	// Update Heal ability cooldown
	if (HealCurrentCooldown > 0)
		HealCurrentCooldown -= deltaTime;
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
	actorRotation.Yaw = lookAt.Yaw + -90.0f;			// Add Yaw offset
	actorRotation.Roll = actorRotation.Pitch = 0.0f;	// Enforce other axis' to 0

	this->SetActorRotation(actorRotation);

	// Also set Controller's look rotation
	m_aiController->SetControlRotation(actorRotation);
}

void ABossCharacter::PerformTaunt()
{
	// Check if taunts are valid
	if (TauntMontages.Num() <= 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No Taunt montages set!"));
		return;
	}

	// If montage in progress, wait
	if (GetWorldTimerManager().IsTimerPending(m_tauntHandle) || m_isTaunting)
	{
		return;
	}

	// Get rnd taunt index and play montage
	int rndTauntIndex = FMath::RandRange(0, TauntMontages.Num() - 1);
	float playDuration = this->PlayAnimMontage(TauntMontages[rndTauntIndex]);

	// Start timer to wait for montage length plus a little cooldown
	float cooldownPeriod = 5.0f;
	float totalDuration = playDuration + cooldownPeriod;
	GetWorldTimerManager().SetTimer(m_tauntHandle, this, &ABossCharacter::OnTauntComplete, totalDuration, false);

	// Log and set taunt flag
	UE_LOG(LogBoss, Log, TEXT("Taunting using '%s' for duration '%f'"), *TauntMontages[rndTauntIndex]->GetName(), totalDuration);
	m_isTaunting = true;
}

void ABossCharacter::OnTauntComplete()
{
	// Taunt is complete, set taunt flag
	UE_LOG(LogBoss, Log, TEXT("Taunt complete"));
	m_isTaunting = false;
}

float ABossCharacter::GetUltimateCooldown()
{
	return UltimateCurrentCd;
}

float ABossCharacter::GetUltimateDamage()
{
	return UltimateDamage;
}

float ABossCharacter::GetMeleeRadius()
{
	return MeleeRadius;
}

float ABossCharacter::GetMeleeDamage()
{
	return (MaxMeleeDamage + MinMeleeDamage) / 2;
}

bool ABossCharacter::IsAttacking()
{
	return m_isAttacking;
}

UCombatStats* ABossCharacter::GetCombatStats()
{
	return m_combatStats;
}

float ABossCharacter::GetAbilityOneCritMultiplier()
{
	return AbilOneCritAmount;
}

bool ABossCharacter::GetIsStunned()
{
	return IsStunned;
}

bool ABossCharacter::GetIsInvisible()
{
	return m_isInvisible;
}

void ABossCharacter::ApplyStun(float duration)
{
	// Play the stun montage if set
	if (StunnedMontage)
	{
		this->PlayAnimMontage(StunnedMontage);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No stun montage set! Stunned but no visual animation"));
	}

	// Set IsStunned
	IsStunned = true;
	GetWorldTimerManager().SetTimer(m_stunHandle, this, &ABossCharacter::EndStun, duration, false);

	UE_LOG(LogBoss, Log, TEXT("Boss is stunned for '%f' seconds"), duration);

	// Cancel Advanced Ability if in progress
	if (GetWorldTimerManager().IsTimerActive(m_rmbDelayHandle))
	{
		GetWorldTimerManager().ClearTimer(m_rmbDelayHandle);
		AdvAttackOnFinish();
	}

	// Cancel Ultimate if in progress
	if (IsPerformingAbility(EAbilities::Ultimate))
	{
		// Set cooldown
		UltimateOnFinish();
	}

	// Cancel heal if in progress
	if (IsPerformingAbility(EAbilities::Heal))
	{
		OnFinishHeal();
	}

	// Cancel melee if boss is in process
	if (IsPerformingAbility(EAbilities::Melee))
	{
		ResetCombo();
	}
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

void ABossCharacter::SetAttackRateDifficulty(float difficulty)
{
	float value = difficulty;
	if (difficulty < 0)
		value = 0.0f;
	else if (difficulty > 1)
		value = 1.0f;
		
	m_attackRateDifficulty = value;
}

float ABossCharacter::GetCurrentDifficultyAttackRate()
{
	return FMath::Lerp(MaxAttackRate, MinAttackRate, m_attackRateDifficulty);
}

float ABossCharacter::GetHealCooldown()
{
	return HealCurrentCooldown;
}

void ABossCharacter::SetMeleeCritMultiplier(float percentMultiplier)
{
	MeleeCritMultiplier = percentMultiplier;
}

void ABossCharacter::PlayCue(USoundBase* sound, float volumeMultiplier, float pitchMultiplier)
{
	if (m_bossAudioComponent)
	{
		// Set volume and pitch
		m_bossAudioComponent->SetVolumeMultiplier(volumeMultiplier);
		m_bossAudioComponent->SetPitchMultiplier(pitchMultiplier);
		
		// Set sound and play
		m_bossAudioComponent->SetSound(sound);
		m_bossAudioComponent->Play();
	}
}

void ABossCharacter::AbilitySucceessful(int abilIndex)
{
	// Add successful ability land
	m_combatStats->AddAbilitySuccess(abilIndex);

	// Trigger event
	if (OnAbilitySucceeded.IsBound())
	{
		OnAbilitySucceeded.Broadcast(abilIndex);
	}
}


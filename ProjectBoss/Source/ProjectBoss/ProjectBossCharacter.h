// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectBossCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeathSignature);

UENUM()
enum EStance
{
	Offensive = 0	UMETA(DisplayName = "Offensive"),
	Evasive = 1		UMETA(DisplayName = "Evasive"),
};

UENUM()
enum EPlayerAbilities
{
	Offensive_AdvAbility = 0		UMETA(DisplayName = "Offensive AdvAbility"),
	Evasive_AdvAbility = 1			UMETA(DisplayName = "Evasive AdvAbility"),

	Offensive_AbilityOne = 2		UMETA(DisplayName = "Offensive AbilityOne"),
	Evasive_AbilityOne = 3			UMETA(DisplayName = "Evasive AbilityOne"),
};

UCLASS(config=Game)
class AProjectBossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AProjectBossCharacter();

	/*
	 *	VARIABLES
	 */
public:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UCapsuleComponent* PoleColliderComponent;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UParticleSystemComponent* PS_PoleStance;

	// Particle system around Wukong's pole when in Offensive
	UPROPERTY(EditAnywhere, Category = "Ability Two")
	class UParticleSystem* OffensivePolePS;
	// Particle system around Wukong's pole when in Evasive
	UPROPERTY(EditAnywhere, Category = "Ability Two")
	class UParticleSystem* EvasivePolePS;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// Current stance of Wukong
	UPROPERTY(VisibleAnywhere, Category = "General")
	TEnumAsByte<EStance> CurrentStance;

	// Current health of the character
	UPROPERTY(VisibleAnywhere, Category = "General")
	float CurrentHealth;
	// Total health of the character
	UPROPERTY(EditAnywhere, Category = "General")
	float TotalHealth;

	// Amount of damage a basic melee attack will deal
	UPROPERTY(EditAnywhere, Category = "Combat")
	float Evasive_MeleeDamageAmount;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float Offensive_MeleeDamageAmount;
	// Current cooldown of melee attack
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float MeleeAtkCurrentCd;
	// Cooldown to perform another melee attack
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeAttackCooldown;

	// Current cooldown of the advanced attack
	UPROPERTY(BlueprintReadOnly, Category = "Advanced Attack")
	float AdvAttackCurrentCd;
	// Amount of damage the offensive advanced attack will deal
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAttackOffensiveDamageAmount;
	// Total cooldown in seconds of the advanced attack
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAttackOffensiveTotalCooldown;
	/// Duration in seconds the cloudwalk ability will last. The duration the player can walk on clouds
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAttackEvasiveCloudDuration;

	// Total cooldown in seconds of ability one
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilityOneTotalCooldown;
	// Amount in units to be the area of effect radius from where the player lands after performing Ability One
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilityOneRadius;
	// Amount of damage Ability One deals to the enemy
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilityOneDamageAmount;
	// Amount in seconds to apply to target if in AoE of stun
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilOneStunDuration;
	// Current cooldown of ability one
	UPROPERTY(BlueprintReadOnly, Category = "Ability One")
	float AbilOneCurrentCd;

	// Total cooldown of Ability Two
	UPROPERTY(EditAnywhere, Category = "Ability Two")
	float AbilityTwoTotalCooldown;
	// Current cooldown of ability two
	UPROPERTY(BlueprintReadOnly, Category = "Ability Two")
	float AbilTwoCurrentCd;

	// Max move speed during offensive stance
	UPROPERTY(EditAnywhere, Category = "Ability Two")
	float Offensive_MaxMS;
	// Attack rate during offensive stance
	UPROPERTY(EditAnywhere, Category = "Ability Two")
	float Offensive_AttackRate;
	// Max move speed when in evasive stance
	UPROPERTY(EditAnywhere, Category = "Ability Two")
	float Evasive_MaxMS;
	// Attack rate during evasive stance
	UPROPERTY(EditAnywhere, Category = "Ability Two")
	float Evasive_AttackRate;

protected:
	// Attack animations to play for a melee attack
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<class UAnimMontage*> AttackAnimMontages;
	// Sound cue's to play when this character lands an attack
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<class USoundBase*> AttackImpactSounds;

	// Montage to play for the Offensive advanced ability
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	class UAnimMontage* AdvancedAttackMontage;
	// Montage to play for the Evasive advanced ability
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	class UAnimMontage* AdvancedEvadeMontage;
	// Sound cue to play for offensive ability one (Shoot fire from staff)
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	class USoundCue* Offensive_AdvAbilSoundCue;
	// Sound cue to play for evasive ability one (Spring into air)
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	class USoundCue* Evasive_AdvAbilSoundCue;

	// Sequence of monatages to use for ability one
	UPROPERTY(EditAnywhere, Category = "Ability One")
	TArray<class UAnimMontage*> AbilityOneMontages;
	// Montage to use for the evasive Ability One
	UPROPERTY(EditAnywhere, Category = "Ability One")
	class UAnimMontage* AbilityOneEvasiveMontage;
	// Voice sound cue to play when trying to perform an ability on cooldown
	UPROPERTY(EditAnywhere, Category = "Ability One")
	class USoundCue* CooldownVoiceCue;
	// Sound cue to play when landing ability one (Slam down to ground, explosion)
	UPROPERTY(EditAnywhere, Category = "Ability One")
	class USoundCue* Offensive_AbilityOneSoundCue;
	// Evasive Ability One sound cue
	//UPROPERTY(EditAnywhere, Category = "Ability One")
	//class USoundCue* Evasive_AbilityOneSoundCue;

	// Grunt sound to use when jumping
	UPROPERTY(EditAnywhere, Category = "Sounds")
	class USoundCue* JumpGruntSoundCue;

private:
	// Time in seconds to save the attack when performing melee montage
	const float SAVE_ATTACK_TIME = 0.59f;
	
	// Core audio component to use for character
	class UAudioComponent* m_audioComponent;
	// Store casted movement component
	class UCharacterMovementComponent* m_charMovementComponent;

	// Track combat stats
	class UCombatStats* m_combatStats;

	// Attacking boolean flags
	bool m_isAttacking;
	bool m_hasAttackedThisSwing;
	bool m_saveAttack;
	float m_attackRate;

	// is character performing any ability
	bool m_isPerformingAbility;
	// Current attack index to use with attack montages
	int m_attackCount;
	// Flag for tracking when character is falling due to ability one
	bool m_offAbilOneIsFalling;

	// Timer Handle for delay destroy of created clouds
	FTimerHandle m_cloudwalkDelayTimer;
	// Array of spawned clouds created during ability
	TArray<AActor*> m_spawnedClouds;
	// Is the character "evading"
	bool m_isEvading;

	/// Disables any walking movement of the character
	bool m_disableLocomotionMovement;

	/*
	*	EVENTS
	*/
public:
	UPROPERTY(BlueprintAssignable)
	FPlayerDeathSignature OnCharacterDied;

	/*
	 * METHODS 
	 */
public:
	EStance GetStance();

	// LMB basic attack
	void PerformMeleeAttack();
	// RMB advanced attack
	void PerformAdvancedAttack();
	// Q ability
	void PerformAbilityOne();
	// E ability
	void PerformAbilityTwo();

	float GetCurrentHealth();
	float GetTotalHealth();
	
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvancedAttackLandDamage();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AbilityOneForceGround();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AbilityOneLandDamage();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AbilityOneEvasiveCloudwalk();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ComboAttackSave();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ResetCombo();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void FinishAbilityOne();

	bool IsEvading();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called when this character takes damage
	virtual float TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser) override;
	// Called when level has finished play
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called every frame
	virtual void Tick(float deltaTime) override;
	// End of APawn interface

private:
	UFUNCTION()
	void OnPoleBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnPoleEndOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CloudwalkDisable();
	// Once player has no more health
	void OnDeath();

	/// <summary>
	/// Sets the current stance of Wukong
	/// </summary>
	/// <param name="targetStance"></param>
	void SetStance(EStance targetStance);

	/// <summary>
	/// Plays a sound through the current audio
	/// </summary>
	/// <param name="sound"></param>
	/// <param name="shouldOverrideExistingSound"></param>
	/// <returns></returns>
	bool PlayCue(class USoundBase* sound, bool shouldOverrideExistingSound = false, float volumeMultiplier = 1.0f, float pitchMultiplier = 1.0f);

	// Callback for when a advanced ability deals damage
	UFUNCTION()
	void AdvAbilityDealtDamage();
	// Callback for when ability one deals damage
	UFUNCTION()
	void AbilOneDealtDamage();

	// Adds a ht marker to the current player HUD
	void HUDAddHitMarker();

	// Converts the target stance to it's string representation
	FString StanceToString(EStance stance);

	// Called when Jump input is pressed
	void OnJump();

	// Core functionality to perform a melee
	void DoMelee();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


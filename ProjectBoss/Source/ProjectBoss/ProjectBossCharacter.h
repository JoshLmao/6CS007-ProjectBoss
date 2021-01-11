// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectBossCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDeathSignature);

UENUM()
enum EStance
{
	Offensive UMETA(DisplayName = "Offensive"),
	Evasive UMETA(DisplayName = "Evasive"),
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
	float MeleeAttackDamageAmount;
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

	// Sound cue to play when landing ability one
	UPROPERTY(EditAnywhere, Category = "Ability One")
	class USoundCue* AbilityOneSoundCue;
	// Total cooldown in seconds of ability one
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilityOneTotalCooldown;
	// Amount in units to be the area of effect radius from where the player lands after performing Ability One
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilityOneRadius;
	// Amount of damage Ability One deals to the enemy
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilityOneDamageAmount;
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilOneStunDuration;
	// Current cooldown of ability one
	UPROPERTY(BlueprintReadOnly, Category = "Ability One")
	float AbilOneCurrentCd;

	// Voice sound cue to play when trying to perform an ability on cooldown
	UPROPERTY(EditAnywhere, Category = "Ability One")
	class USoundCue* CooldownVoiceCue;

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
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<class UAnimMontage*> AttackAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	class UAnimMontage* AdvancedAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	class UAnimMontage* AdvancedEvadeMontage;

	UPROPERTY(EditAnywhere, Category = "Ability One")
	TArray<class UAnimMontage*> AbilityOneMontages;

	UPROPERTY(EditAnywhere, Category = "Ability One")
	class UAnimMontage* AbilityOneEvasiveMontage;

private:
	class UAudioComponent* m_audioComponent;
	class UCharacterMovementComponent* m_charMovementComponent;

	bool m_isAttacking;
	bool m_hasAttackedThisSwing;
	bool m_saveAttack;
	float m_attackRate;

	bool m_isPerformingAbility;
	int m_attackCount;
	bool m_offAbilOneIsFalling;

	FTimerHandle m_cloudwalkDelayTimer;
	TArray<AActor*> m_spawnedClouds;

	bool m_isEvading;

	/// <summary>
	/// Disables any walking movement of the character
	/// </summary>
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
	bool PlayCue(class USoundBase* sound, bool shouldOverrideExistingSound = false);

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


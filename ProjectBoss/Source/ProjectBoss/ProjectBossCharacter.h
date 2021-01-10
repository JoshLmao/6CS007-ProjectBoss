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

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UParticleSystem* OffensivePolePS;
	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UParticleSystem* EvasivePolePS;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

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

	// Sound cue to play when 
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

protected:
	UPROPERTY(EditAnywhere, Category = "Project Boss")
	TArray<class UAnimMontage*> AttackAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UAnimMontage* AdvancedAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UAnimMontage* AdvancedEvadeMontage;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	TArray<class UAnimMontage*> AbilityOneMontages;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UAnimMontage* AbilityOneEvasiveMontage;

	UPROPERTY(VisibleAnywhere, Category = "Project Boss")
	TEnumAsByte<EStance> CurrentStance;

	float CurrentHealth;
	float TotalHealth;

private:
	const float STANCE_EVASIVE_ATTACK_RATE = 0.75f;
	const float STANCE_OFFENSIVE_ATTACK_RATE = 1.50f;

	class UAudioComponent* m_audioComponent;

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


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

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	// LMB Basic Attack
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeAttackDamageAmount;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeAtkCurrentCd;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float MeleeAttackCooldown;

	// RMB Advanced Attack
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float AdvAttackCurrentCd;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AdvAttackOffensiveDamageAmount;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AdvAttackOffensiveTotalCooldown;

	// Q Ability One
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AbilityOneTotalCooldown;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AbilityOneRadius;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AbilityOneDamageAmount;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float AbilOneCurrentCd;

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
	bool m_isAttacking;
	bool m_hasAttackedThisSwing;
	bool m_saveAttack;
	bool m_isPerformingAbility;
	int m_attackCount;
	bool m_offAbilOneIsFalling;

	/// <summary>
	/// Disables any walking movement of the character
	/// </summary>
	bool m_disableLocomotionMovement;
	
	FTimerHandle m_cloudwalkDelayTimer;
	TArray<AActor*> m_spawnedClouds;

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

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


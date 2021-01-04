// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ProjectBossCharacter.generated.h"

UCLASS(config=Game)
class AProjectBossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AProjectBossCharacter();

	/*
	 * Variables
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

	// RMB Advanced Attack
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AdvAttackTotalCooldown;
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	float AdvAttackCurrentCd;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AdvAttackDamageAmount;

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
	TArray<class UAnimMontage*> AbilityOneMontages;

private:
	bool m_isAttacking;
	bool m_hasAttackedThisSwing;
	bool m_saveAttack;
	int m_attackCount;

	/// <summary>
	/// Disables any walking movement of the character
	/// </summary>
	bool m_disableLocomotionMovement;

	/*
	 * Methods 
	 */
public:
	// LMB basic attack
	void PerformMeleeAttack();
	// RMB advanced attack
	void PerformAdvancedAttack();
	// Q ability
	void PerformAbilityOne();
	
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvancedAttackLandDamage();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AbilityOneForceGround();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AbilityOneLandDamage();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ComboAttackSave();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ResetCombo();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void FinishAbilityOne();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

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

protected:
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
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};


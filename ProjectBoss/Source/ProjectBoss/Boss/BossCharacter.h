// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBossDeathSignature);

UCLASS()
class PROJECTBOSS_API ABossCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABossCharacter();

	/*
	*	VARIABLES
	*/

protected:
	// Basic melee attack montages
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	TArray<class UAnimMontage*> AttackAnimMontages;

	// Montages for performing the advanced attack
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	TArray<class UAnimMontage*> AdvancedAttackMontages;

	// Montages for performing Ability One
	UPROPERTY(EditAnywhere, Category = "Ability One")
	TArray<class UAnimMontage*> AbilityOneMontages;
	// Invisible material instance to apply to mesh while active
	UPROPERTY(EditAnywhere, Category = "Ability One")
	class UMaterialInstance* InvisibleMatInst;

	// Montages for performing the ultimate ability
	UPROPERTY(EditAnywhere, Category = "Ability Ultimate")
	TArray<class UAnimMontage*> AbilityUltimateMontages;

	// Capsule component for the left blade
	UPROPERTY(EditAnywhere, Category = "General")
	class UCapsuleComponent* LeftBladeCollider;
	// Capsule component for the right blade
	UPROPERTY(EditAnywhere, Category = "General")
	class UCapsuleComponent* RightBladeCollider;

	// Throwable Dagger blueprint for Ability One
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	TSubclassOf<class AThrowableDagger> ThrowableDagger;

	// Total Health of the boss
	UPROPERTY(EditAnywhere, Category = "General")
	float TotalHealth;
	// The current health of the boss
	UPROPERTY(BlueprintReadOnly, Category = "General")
	float CurrentHealth;

	// Amount of damage a melee attack deals
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	float MeleeDamage;

	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAbilityDamage;
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAbilityCooldown;

	// Amount of damage the ultimate deals
	UPROPERTY(EditAnywhere, Category = "Ability Ultimate")
	float UltimateDamage;
	// Total cooldown in seconds of the ultimate
	UPROPERTY(EditAnywhere, Category = "Ability Ultimate")
	float UltimateCooldown;

private:
	// Melee Attack
	bool m_isAttacking;
	bool m_saveAttack;
	int m_attackCount;
	bool m_dmgThisAttack;

	// Advanced Attack
	AActor* m_rmbTarget;
	FTimerHandle m_rmbDelayHandle;
	bool m_rmbAimAtPlayer;
	float m_rmbCurrentCooldown;

	// Ability one
	TArray<class UMaterialInstanceDynamic*> m_originalMeshMaterials;
	class UMaterialInstanceDynamic* m_invisMatInst;

	// Ability Ultimate
	AActor* m_ultiTargetActor;
	float m_ultiCurrentCooldown;
	bool m_ultiIsChanneling;

	class AAIController* m_aiController;

	/**  Events  **/
public:
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ComboAttackSave();
	
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ResetCombo();

	UPROPERTY(BlueprintAssignable)
	FBossDeathSignature OnCharacterDied;

	/**  Methods  **/
public:	
	// Called every frame
	virtual void Tick(float deltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called when this enemy takes damage
	virtual float TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser) override;

	/// <summary>
	/// Executes a melee attack
	/// </summary>
	void PerformMeleeAttack();
	/// <summary>
	/// Performs the RMB advanced attack
	/// </summary>
	/// <param name="target"></param>
	void PerformAdvancedAttack(AActor* target);
	
	// Sets the materials of the boss to be invisible
	void SetInvisible(bool isInvis);
	// Chases a target, returns true if reached the target
	bool ChaseTarget(AActor* target);

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvAttackFinishedPrepare();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvAttackOnThrowDagger();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvAttackOnReleaseDagger();

	void PerformUltimate(AActor* target);

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void UltimateTeleportTo();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetTotalHealth();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetUltimateCooldown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBladeBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnDeath();

	void LookAtActor(AActor* target);
};

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
public:
	// Is the character currently stunned
	bool IsStunned;

protected:
	// Basic melee attack montages
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	TArray<class UAnimMontage*> AttackAnimMontages;
	// Amount of damage a melee attack deals
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	float MeleeDamage;

	// Montages for performing the advanced attack
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	TArray<class UAnimMontage*> AdvancedAttackMontages;
	// Throwable Dagger blueprint for Ability One
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	TSubclassOf<class AThrowableDagger> ThrowableDagger;
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAbilityDamage;
	UPROPERTY(BlueprintReadOnly, Category = "Advanced Attack")
	float AdvAbilityCurrentCd;
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAbilityTotalCooldown;

	// Montages for performing Ability One
	UPROPERTY(EditAnywhere, Category = "Ability One")
	TArray<class UAnimMontage*> AbilityOneMontages;
	// Invisible material instance to apply to mesh while active
	UPROPERTY(EditAnywhere, Category = "Ability One")
	class UMaterialInstance* InvisibleMatInst;
	// Percent crit damage to apply if attack at the end of ability one
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilOneCritAmount;
	// Current cooldown of ability one
	UPROPERTY(BlueprintReadOnly, Category = "Ability One")
	float AbilOneCurrentCd;
	// Total cooldown for using ability one
	UPROPERTY(EditAnywhere, Category = "Ability One")
	float AbilOneTotalCooldown;

	// Montages for performing the ultimate ability
	UPROPERTY(EditAnywhere, Category = "Ability Ultimate")
	TArray<class UAnimMontage*> AbilityUltimateMontages;
	// Amount of damage the ultimate deals
	UPROPERTY(EditAnywhere, Category = "Ability Ultimate")
	float UltimateDamage;
	// Total cooldown in seconds of the ultimate
	UPROPERTY(EditAnywhere, Category = "Ability Ultimate")
	float UltimateTotalCooldown;
	UPROPERTY(BlueprintReadOnly, Category = "Ability Ultimate")
	float UltimateCurrentCd;

	// Capsule component for the left blade
	UPROPERTY(EditAnywhere, Category = "General")
	class UCapsuleComponent* LeftBladeCollider;
	// Capsule component for the right blade
	UPROPERTY(EditAnywhere, Category = "General")
	class UCapsuleComponent* RightBladeCollider;
	UPROPERTY(EditAnywhere, Category = "General")
	class USoundCue* DeathCue;
	// Total Health of the boss
	UPROPERTY(EditAnywhere, Category = "General")
	float TotalHealth;
	// The current health of the boss
	UPROPERTY(BlueprintReadOnly, Category = "General")
	float CurrentHealth;

	// Montage to play while character is stunned
	UPROPERTY(EditAnywhere, Category = "Montages")
	class UAnimMontage* StunnedMontage;

private:
	class UAudioComponent* m_bossAudioComponent;
	class AAIController* m_aiController;

	bool m_isPerformingAnyAbility;

	// Melee Attack
	bool m_isAttacking;
	bool m_saveAttack;
	int m_attackCount;
	bool m_dmgThisAttack;

	// Advanced Attack
	AActor* m_rmbTarget;
	FTimerHandle m_rmbDelayHandle;
	bool m_rmbAimAtPlayer;

	// Ability one
	TArray<class UMaterialInstanceDynamic*> m_originalMeshMaterials;
	class UMaterialInstanceDynamic* m_invisMatInst;
	bool m_isInvisible;

	// Ability Ultimate
	AActor* m_ultiTargetActor;
	bool m_ultiIsChanneling;

	FTimerHandle m_stunHandle;

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
	// Chases a target and returns the value of the pathfinding attempt
	bool ChaseTarget(AActor* target);
	// Stops the chase target
	void CancelChaseTarget();

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
	float GetAdvancedAbilityCooldown();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetAbilityOneCooldown();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetUltimateCooldown();

	// Stuns the target for the duration in seconds
	void ApplyStun(float duration);

	// -1 for any, 0 melee, 1 advanced, 2 ability one, 3 ultimate
	bool IsPerformingAbility(int abilIndex = -1);
	// Get if the character is currently stunned
	bool GetIsStunned();

	// Begins the cooldown for ability one
	void BeginAbilityOneCooldown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBladeBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void OnDeath();

	void LookAtActor(AActor* target);

	void EndStun();

	// Gets if the boss can perform an ability or melee action
	bool CanPerformAction();
};

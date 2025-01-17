// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BossCharacter.generated.h"

UENUM()
enum EAbilities
{
	Any = -1		UMETA(DisplayName = "Any"),
	Melee = 0		UMETA(DisplayName = "Melee"),
	Advanced = 1	UMETA(DisplayName = "Advanced"),
	One = 2			UMETA(DisplayName = "One"),
	Ultimate = 3	UMETA(DisplayName = "Ultimate"),
	Heal = 4		UMETA(DisplayName = "Heal"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBossDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilitySucceedSignature, int, abilityIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMeleeSucceedSignature);

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
	// Array of sounds to play when character lands an attack
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	TArray<class USoundBase*> AttackSoundCues;

	// Minimum amount of damage a melee attack deals
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	float MinMeleeDamage;
	// Maximum amount of damage a melee attack deals
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	float MaxMeleeDamage;
	// Amount in units to be within radius of target to perform a melee attack
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	float MeleeRadius;
	// Slowest attack rate the character attacks/melees the target
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	float MinAttackRate;
	// Fastest attack rate the character attacks/melees the target
	UPROPERTY(EditAnywhere, Category = "Melee Attack")
	float MaxAttackRate;


	// Montages for performing the advanced attack
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	TArray<class UAnimMontage*> AdvancedAttackMontages;
	// Throwable Dagger blueprint for Ability One
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	TSubclassOf<class AThrowableDagger> ThrowableDagger;
	// Amount of damage the advanced ability does to the target
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAbilityDamage;
	// Current cooldown of the avanced ability
	UPROPERTY(BlueprintReadOnly, Category = "Advanced Attack")
	float AdvAbilityCurrentCd;
	// Total cooldown of the advanced ability
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAbilityTotalCooldown;
	// Minimum and Maximum distance to perform advanced ability
	UPROPERTY(EditAnywhere, Category = "Advanced Attack")
	float AdvAbilityMaxRange;

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
	// The current cooldown of the ultimate ability
	UPROPERTY(BlueprintReadOnly, Category = "Ability Ultimate")
	float UltimateCurrentCd;

	// Montage to play when healing
	UPROPERTY(EditAnywhere, Category = "Ability Heal")
	class UAnimMontage* HealMontage;
	// Percent to heal
	UPROPERTY(EditAnywhere, Category = "Ability Heal")
	float HealPercent;
	// Time in seconds to spread the heal ability over
	UPROPERTY(EditAnywhere, Category = "Ability Heal")
	float HealDuration;
	// Amount in health required the character to be <= to perform heal
	UPROPERTY(EditAnywhere, Category = "Ability Heal")
	float PerformHealThreshold;
	// Total cooldown time in seconds of the heal 
	UPROPERTY(EditAnywhere, Category = "Ability Heal")
	float HealTotalCooldown;
	// Current cooldown of the heal ability
	UPROPERTY(BlueprintReadOnly, Category = "Ability Heal")
	float HealCurrentCooldown;

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
	// Percentage crit multiplier for the next melee attack
	UPROPERTY(EditAnywhere, Category = "General")
	float MeleeCritMultiplier;

	// Montage to play while character is stunned
	UPROPERTY(EditAnywhere, Category = "Montages")
	class UAnimMontage* StunnedMontage;

	// All taunt montages for the character
	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<class UAnimMontage*> TauntMontages;

private:
	// Audio Component to use to broadcast audio from this character
	class UAudioComponent* m_bossAudioComponent;
	// AI controller class controlling this character
	class AAIController* m_aiController;

	// Is the character performing any ability?
	bool m_isPerformingAnyAbility;

	// Is the character currently attacking?
	bool m_isAttacking;
	// Should the next attack be saved?
	bool m_saveAttack;
	// Current index to use when selecting an attack montage
	int m_attackCount;
	// Did damage get dealt this attack swing?
	bool m_dmgThisAttack;
	// Value from zero to one that influences where between the Min and Max attack rate to set the attack rate
	float m_attackRateDifficulty;

	//	The actor to aim at when performing the RMB ability
	UPROPERTY()
	AActor* m_rmbTarget;
	// RMB ability's delay timer handle
	FTimerHandle m_rmbDelayHandle;
	// Is the character currently aiming at the player?
	bool m_rmbAimAtPlayer;

	// Material instances of the original mesh's mats
	UPROPERTY()
	TArray<class UMaterialInstanceDynamic*> m_originalMeshMaterials;
	// Material instance to apply to make character invisible
	UPROPERTY()
	class UMaterialInstanceDynamic* m_invisMatInst;

	// Is the Boss currently invisible?
	bool m_isInvisible;
	// Is the boss performing the ability one attack?
	bool m_abilOneIsAttacking;

	// Ability Ultimate
	AActor* m_ultiTargetActor;
	// is the ultimate ability being channeled?
	bool m_ultiIsChanneling;

	// Ability Heal
	float m_healTimeRemaining;
	// Stun timer handle
	FTimerHandle m_stunHandle;
	// Combat statistics tracking for the boss character
	UPROPERTY()
	class UCombatStats* m_combatStats;

	// Taunt timer handle for when taunts are finished
	FTimerHandle m_tauntHandle;
	// is the character currently taunting?
	bool m_isTaunting;

	/**  Events  **/
public:
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ComboAttackSave();
	
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void ResetCombo();

	// Event the triggers when this character has died
	UPROPERTY(BlueprintAssignable)
	FBossDeathSignature OnCharacterDied;

	// Event triggered when an ability succeeds
	UPROPERTY(BlueprintAssignable)
	FAbilitySucceedSignature OnAbilitySucceeded;

	// Triggered when a melee is successful
	UPROPERTY(BlueprintAssignable)
	FMeleeSucceedSignature OnMeleeSucceeded;

	/**  Methods  **/
public:	
	// Called every frame
	virtual void Tick(float deltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called when this enemy takes damage
	virtual float TakeDamage(float damageAmount, struct FDamageEvent const& damageEvent, class AController* eventInstigator, AActor* damageCauser) override;

	// Executes a melee attack
	void PerformMeleeAttack();
	// Performs the RMB advanced attack
	void PerformAdvancedAttack(AActor* target);
	// Performs the Ultimate ability
	void PerformUltimate(AActor* target);

	// Performs a self heal ability
	void PerformHeal();
	// If the boss is within range to perform heal ability
	bool CanHeal();

	// Sets the materials of the boss to be invisible
	void SetInvisible(bool isInvis);
	// Chases a target and returns the value of the pathfinding attempt
	bool MoveToLocation(FVector location);
	// Stops any move to location command
	void CancelMoveToLocation();
	// Rotates actor to look at location
	void LookAtActor(FVector location);

	// Performs a random taunt
	void PerformTaunt();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvAttackFinishedPrepare();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvAttackOnThrowDagger();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void AdvAttackOnReleaseDagger();
	void AdvAttackOnFinish();

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void UltimateTeleportTo();
	void UltimateOnFinish();

	// Get the character's current health
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetCurrentHealth();
	// Gets the total health of the character
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetTotalHealth();

	// Gets the current cooldown of the advanced ability
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetAdvancedAbilityCooldown();
	// Gets the damage amount for Advanced Ability
	float GetAdvancedAbilityDamage();

	// Gets the current cooldown of ability one
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetAbilityOneCooldown();

	// Gets the current cooldown of the ultimate
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetUltimateCooldown();
	// Gets the damage amount for Ultimate ability
	float GetUltimateDamage();

	// Gets current cooldown of heal ability
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetHealCooldown();

	// Sets a percentage multiplier to the next melee attack
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	void SetMeleeCritMultiplier(float percentMultiplier);

	// Gets the distance in units to be in range to melee
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetMeleeRadius();
	// Gets the average melee damage
	float GetMeleeDamage();
	// Gets if the character is currently attacking
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	bool IsAttacking();

	// Gets the pointer to the combat statistics of the boss
	UCombatStats* GetCombatStats();

	// Stuns the target for the duration in seconds
	void ApplyStun(float duration);

	float GetAbilityOneCritMultiplier();

	// -1 for any, 0 melee, 1 advanced, 2 ability one, 3 ultimate
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	bool IsPerformingAbility(int abilIndex = -1);
	// Get if the character is currently stunned
	bool GetIsStunned();
	// Get if the character is currently invisible
	bool GetIsInvisible();

	// Begins the cooldown for ability one
	void BeginAbilityOneCooldown();

	// Set the attack rate difficulty of the character. Difficulty can range between 1 and 0
	void SetAttackRateDifficulty(float difficulty);
	// Gets the current attack rate between the min and max, including the difficulty
	float GetCurrentDifficultyAttackRate();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when controller has been set on the character
	virtual void PossessedBy(AController* NewController) override;

	// Called when end of play for level
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnBladeBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void UpdateAbilityCooldowns(float deltaTime);

	void OnDeath();

	void DoMelee();

	void EndStun();

	// Gets if the boss can perform an ability or melee action
	bool CanPerformAction();

	// Called when heal ability has finished
	void OnFinishHeal();

	UFUNCTION()
	void OnAdvAttackDealtDamage();

	// Plays a sound cue
	void PlayCue(USoundBase* sound, float volumeMultiplier = 1.0f, float pitchMultiplier = 1.0f);
	// When an ability succeeds, run relevant code (event, etc)
	void AbilitySucceessful(int abilIndex);

	void DoHeal(float deltaTime);

	void OnTauntComplete();
};

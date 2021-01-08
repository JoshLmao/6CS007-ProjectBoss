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
	UPROPERTY(EditAnywhere, Category = "Project Boss")
	TArray<class UAnimMontage*> AttackAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	TArray<class UAnimMontage*> AdvancedAttackMontages;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	TArray<class UAnimMontage*> AbilityUltimateMontages;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UCapsuleComponent* LeftBladeCollider;
	UPROPERTY(EditAnywhere, Category = "Project Boss")
	class UCapsuleComponent* RightBladeCollider;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	TSubclassOf<class AThrowableDagger> ThrowableDagger;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	float TotalHealth;
	UPROPERTY(BlueprintReadOnly, Category = "Project Boss")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	float MeleeDamage;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	float UltimateDamage;
	UPROPERTY(EditAnywhere, Category = "Project Boss")
	float UltimateCooldown;

private:
	bool m_isAttacking;
	bool m_saveAttack;
	int m_attackCount;
	bool m_dmgThisAttack;

	AActor* m_rmbTarget;
	FTimerHandle m_rmbDelayHandle;
	bool m_rmbAimAtPlayer;

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

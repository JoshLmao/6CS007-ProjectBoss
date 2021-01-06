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

	/**  Variables  **/

protected:
	UPROPERTY(EditAnywhere, Category = "Project Boss")
	TArray<UAnimMontage*> AttackAnimMontages;

	UPROPERTY(EditAnywhere, Category = "Project Boss")
	float TotalHealth;
	UPROPERTY(BlueprintReadOnly, Category = "Project Boss")
	float CurrentHealth;

private:
	bool m_isAttacking;
	bool m_saveAttack;
	int m_attackCount;

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

	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetCurrentHealth();
	UFUNCTION(BlueprintCallable, Category = "Project Boss")
	float GetTotalHealth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void OnDeath();
};

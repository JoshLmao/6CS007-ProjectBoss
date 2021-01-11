// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CapsuleAOEDamage.generated.h"

UCLASS()
class PROJECTBOSS_API ACapsuleAOEDamage : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapsuleAOEDamage();

	class UCapsuleComponent* CapsuleComponent;

private:
	float m_damageAmount;
	float m_stunDuration;
	bool m_hasAppliedDmg;

	class AActor* CauserActor;
	class AController* CauserController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Set the capsule radius and half height of the aoe damage
	void ConfigureCapsule(float radius, float halfHeight);
	// Set the damage source
	void ConfigureDamage(float dmgAmount, class AController* causerController, class AActor* causerActor);
	// Should the AOE damage bubble apply a stun to the target
	void SetStunDuration(float duration);

private:
	UFUNCTION()
	void OnAOEBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

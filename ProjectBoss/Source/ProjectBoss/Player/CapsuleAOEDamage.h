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
	float DamageAmount;
	class AActor* CauserActor;
	class AController* CauserController;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ConfigureCapsule(float radius, float halfHeight);
	void ConfigureDamage(float dmgAmount, class AController* causerController, class AActor* causerActor);

private:
	UFUNCTION()
	void OnAOEBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

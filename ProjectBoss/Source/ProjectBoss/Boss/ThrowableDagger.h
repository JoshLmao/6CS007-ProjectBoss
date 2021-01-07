// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableDagger.generated.h"

UCLASS()
class PROJECTBOSS_API AThrowableDagger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowableDagger();

	/*
	*	VARIABLES
	*/

private:
	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* CollisionComponent;
	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* MeshComponent;

	float m_damageAmount;
	FVector m_movementDirection;
	float m_speed;

	/*
	*	METHODS
	*/
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetDamage(float damage);
	void SetMovementDirection(FVector targetVector, float speed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnDaggerBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


};

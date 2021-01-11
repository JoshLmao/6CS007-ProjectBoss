// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CloudwalkCloud.generated.h"

UENUM()
enum EDirections
{
	None UMETA(DisplayName = "None"),
	North UMETA(DisplayName = "North"),
	East UMETA(DisplayName = "East"),
	South UMETA(DisplayName = "South"),
	West UMETA(DisplayName = "West"),
};

UCLASS()
class PROJECTBOSS_API ACloudwalkCloud : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACloudwalkCloud();

	/*
	*	VARIABLES
	*/

protected:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* CloudFloor;

	UPROPERTY(EditAnywhere)
	class UParticleSystemComponent* FloorPS;

private:
	UPROPERTY(EditAnywhere)
	class UBoxComponent* m_westWallCollider;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* m_eastWallCollider;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* m_northWallCollider;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* m_southWallCollider;

	bool m_shouldCollide;
	bool m_spawnedNorth;
	bool m_spawnedSouth;
	bool m_spawnedEast;
	bool m_spawnedWest;

	TArray<EDirections> m_disableDirections;
	TArray<AActor*>* m_arrayPtr;
	ACloudwalkCloud* m_nextCloud;
	/*
	*	METHODS
	*/

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DisableDirection(EDirections direction);

	void SetTrackingArray(TArray<AActor*>* arrayPtr);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnEastOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnWestOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnNorthOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSouthOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void SpawnNewCloud(EDirections spawnDirection, FVector spawnOffset);
	bool ValidateOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp);
	bool HasSpawnedInAnyDirection();
};

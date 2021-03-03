// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossSpawnTrigger.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnedBossSignature, AActor*, boss);

UCLASS()
class PROJECTBOSS_API ABossSpawnTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABossSpawnTrigger();

public:
	// Size of spawn trigger
	UPROPERTY(EditAnywhere)
	FVector BoxExtent;

	// Actor of target to specify where to spawn, use it's location
	UPROPERTY(EditAnywhere)
	AActor* SpawnLocation;

	// Actor to spawn (The boss)
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABossCharacter> BossActor;

private:
	// Box trigger component
	class UBoxComponent* SpawnTrigger;
	// has the boss already been spanwed?
	bool m_hasSpawnedBoss;

public:
	// Event for when the boss gets spawned in the world
	UPROPERTY(BlueprintAssignable)
	FSpawnedBossSignature OnSpawnedBoss;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void OnTriggerOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};

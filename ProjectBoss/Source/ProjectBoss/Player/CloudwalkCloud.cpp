// Fill out your copyright notice in the Description page of Project Settings.


#include "CloudwalkCloud.h"
#include "Components/BoxComponent.h"
#include "../ProjectBossCharacter.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACloudwalkCloud::ACloudwalkCloud()
{
	m_shouldCollide = false;
	m_spawnedNorth = m_spawnedSouth = m_spawnedEast = m_spawnedWest = false;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CloudFloor = CreateDefaultSubobject<UBoxComponent>(TEXT("CloudFloor"));
	RootComponent = CloudFloor;

	// Configure main cloud for walking on
	CloudFloor->SetCollisionProfileName("BlockAll", true);
	CloudFloor->SetBoxExtent(FVector(200.0f, 200.0f, 0.5f), true);

	FloorPS = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FloorParticleSystem"));
	FloorPS->SetupAttachment(CloudFloor);
	FloorPS->SetRelativeLocation(FVector(0, 0, -125.0f));
	float scale = 1.75;
	FloorPS->SetRelativeScale3D(FVector(scale, scale, scale));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> cloudParticles(TEXT("/Game/ParagonSunWukong/FX/Particles/Wukong/Abilities/DoubleJump/FX/p_CloudBurst"));
	if (cloudParticles.Succeeded())
	{
		FloorPS->SetTemplate(cloudParticles.Object);
	}


	if (!m_disableDirections.Contains( EDirections::East))
	{
		m_eastWallCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("EastWallCollider"));
		m_eastWallCollider->SetupAttachment(CloudFloor);
		m_eastWallCollider->SetCollisionProfileName("OverlapAllDynamic", false);
		m_eastWallCollider->OnComponentBeginOverlap.AddDynamic(this, &ACloudwalkCloud::OnEastOverlap);
		m_eastWallCollider->SetBoxExtent(FVector(200.0f, 0.5f, 200.0f), false);
		m_eastWallCollider->AddLocalOffset(FVector(0, 200.0f, 200.0f));
	}

	if (!m_disableDirections.Contains(EDirections::West))
	{
		m_westWallCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("WestWallCollider"));
		m_westWallCollider->SetupAttachment(CloudFloor);
		m_westWallCollider->SetCollisionProfileName("OverlapAllDynamic", false);
		m_westWallCollider->OnComponentBeginOverlap.AddDynamic(this, &ACloudwalkCloud::OnWestOverlap);
		m_westWallCollider->SetBoxExtent(FVector(200.0f, 0.5f, 200.0f), false);
		m_westWallCollider->AddLocalOffset(FVector(0, -200.0f, 200.0f));
	}

	if (!m_disableDirections.Contains(EDirections::South))
	{
		m_southWallCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("SouthWallCollider"));
		m_southWallCollider->SetupAttachment(CloudFloor);
		m_southWallCollider->SetCollisionProfileName("OverlapAllDynamic", false);
		m_southWallCollider->OnComponentBeginOverlap.AddDynamic(this, &ACloudwalkCloud::OnSouthOverlap);
		m_southWallCollider->SetBoxExtent(FVector(0.5f, 200.0f, 200.0f), false);
		m_southWallCollider->AddLocalOffset(FVector(-200.0f, 0, 200.0f));
	}

	if (!m_disableDirections.Contains(EDirections::North))
	{
		m_northWallCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("NorthWallCollider"));
		m_northWallCollider->SetupAttachment(CloudFloor);
		m_northWallCollider->SetCollisionProfileName("OverlapAllDynamic", true);
		m_northWallCollider->OnComponentBeginOverlap.AddDynamic(this, &ACloudwalkCloud::OnNorthOverlap);
		m_northWallCollider->SetBoxExtent(FVector(0.5f, 200.0f, 200.0f), true);
		m_northWallCollider->AddLocalOffset(FVector(200.0f, 0, 200.0f));
	}

	// Enable to be seen in game for debug
	/*
	* CloudFloor->SetHiddenInGame(false)
	* m_westWallCollider->SetHiddenInGame(false);
	* m_eastWallCollider->SetHiddenInGame(false);
	* m_northWallCollider->SetHiddenInGame(false);
	* m_southWallCollider->SetHiddenInGame(false);
	*/
}

// Called when the game starts or when spawned
void ACloudwalkCloud::BeginPlay()
{
	Super::BeginPlay();
	m_shouldCollide = true;
}

// Called every frame
void ACloudwalkCloud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACloudwalkCloud::OnEastOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_disableDirections.Contains(EDirections::East))
	{
		return;
	}
	if (m_nextCloud)
	{
		m_nextCloud->DisableDirection(EDirections::West);
		return;
	}
	if (ValidateOverlap(OverlappedComp, OtherActor, OtherComp))
	{
		if (!HasSpawnedInAnyDirection())
		{
			UE_LOG(LogTemp, Log, TEXT("Player collided with East"));
			SpawnNewCloud(EDirections::West, FVector(0, 300.0f, 0));
			m_spawnedEast = true;
		}
	}
}

void ACloudwalkCloud::OnWestOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_disableDirections.Contains(EDirections::West))
	{
		return;
	}
	if (m_nextCloud)
	{
		m_nextCloud->DisableDirection(EDirections::East);
		return;
	}
	if (ValidateOverlap(OverlappedComp, OtherActor, OtherComp))
	{
		if (!HasSpawnedInAnyDirection())
		{
			UE_LOG(LogTemp, Log, TEXT("Player collided with West"));
			SpawnNewCloud(EDirections::East, FVector(0, -300.0f, 0));
			m_spawnedWest = true;
		}
	}
	
}
void ACloudwalkCloud::OnNorthOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_disableDirections.Contains(EDirections::North))
	{
		return;
	}
	if (m_nextCloud)
	{
		m_nextCloud->DisableDirection(EDirections::South);
		return;
	}
	if (ValidateOverlap(OverlappedComp, OtherActor, OtherComp))
	{
		if (!HasSpawnedInAnyDirection())
		{
			UE_LOG(LogTemp, Log, TEXT("Player collided with North"));
			SpawnNewCloud(EDirections::South, FVector(300.0f, 0, 0));
			m_spawnedNorth = true;
		}
	}
	
}
void ACloudwalkCloud::OnSouthOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (m_disableDirections.Contains(EDirections::South))
	{
		return;
	}
	if (m_nextCloud)
	{
		m_nextCloud->DisableDirection(EDirections::North);
		return;
	}

	if (ValidateOverlap(OverlappedComp, OtherActor, OtherComp))
	{
		if (!HasSpawnedInAnyDirection())
		{
			UE_LOG(LogTemp, Log, TEXT("Player collided with South"));
			SpawnNewCloud(EDirections::North, FVector(-300.0f, 0, 0));
			m_spawnedSouth = true;
		}
	}
}

void ACloudwalkCloud::SpawnNewCloud(EDirections disableDirection, FVector spawnOffset)
{
	if (!m_shouldCollide)
		return;
	
	FVector oldCloudLocation = GetActorLocation();
	FVector newLocation =  oldCloudLocation + spawnOffset;

	AActor* newActor = GetWorld()->SpawnActor<ACloudwalkCloud>(ACloudwalkCloud::StaticClass(), FActorSpawnParameters());
	
	if (m_arrayPtr)
		m_arrayPtr->Add(newActor);

	UE_LOG(LogTemp, Log, TEXT("Created a new Cloud"));

	ACloudwalkCloud* newCloud = Cast<ACloudwalkCloud>(newActor);
	newCloud->DisableDirection(disableDirection);
	newCloud->SetActorLocation(newLocation, false);
	newCloud->SetTrackingArray(m_arrayPtr);

	m_nextCloud = newCloud;
	//DrawDebugBox(GetWorld(), newLocation, FVector(200.0f, 200.0f, 0.5f), FColor::Red, false, 10.0f, 0, 5.0f);
}

void ACloudwalkCloud::DisableDirection(EDirections direction)
{
	m_disableDirections.Add(direction);

	switch (direction)
	{
		case EDirections::East:
			m_eastWallCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			m_eastWallCollider->SetCollisionProfileName("NoCollision", true);
			break;
		case EDirections::West:
			m_westWallCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			m_westWallCollider->SetCollisionProfileName("NoCollision", true);
			break;
		case EDirections::North:
			m_northWallCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			m_northWallCollider->SetCollisionProfileName("NoCollision", true);
			break;
		case EDirections::South:
			m_southWallCollider->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			m_southWallCollider->SetCollisionProfileName("NoCollision", true);
			break;
	}
}

void ACloudwalkCloud::SetTrackingArray(TArray<AActor*>* arrayPtr)
{
	m_arrayPtr = arrayPtr;
}

bool ACloudwalkCloud::ValidateOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp)
{
	// Be false if spawned another cloud already
	if (m_spawnedNorth || m_spawnedEast || m_spawnedSouth || m_spawnedWest)
	{
		return false;
	}
	if (OtherComp == m_northWallCollider || OtherComp == m_southWallCollider || OtherComp == m_eastWallCollider || OtherComp == m_westWallCollider || OtherComp == CloudFloor)
	{
		return false;
	}

	// Ignore existing box colliders
	if (OtherComp->IsA(UBoxComponent::StaticClass()) || OtherComp->GetName() == "PoleColliderComponent" )
	{
		return false;
	}

	if (OtherActor->IsA(AProjectBossCharacter::StaticClass()))
	{
		return true;
	}

	return false;
}

bool ACloudwalkCloud::HasSpawnedInAnyDirection()
{
	return m_spawnedNorth || m_spawnedSouth || m_spawnedEast || m_spawnedWest;
}

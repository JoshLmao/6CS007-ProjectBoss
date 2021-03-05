// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSpawnTrigger.h"
#include "Components/BoxComponent.h"
#include "../ProjectBossCharacter.h"
#include "../Boss/BossCharacter.h"

// Sets default values
ABossSpawnTrigger::ABossSpawnTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxExtent = FVector(100, 100, 100);

	SpawnTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Trigger"));
	RootComponent = SpawnTrigger;
	SpawnTrigger->OnComponentBeginOverlap.AddDynamic(this, &ABossSpawnTrigger::OnTriggerOverlap);
	//SpawnTrigger->bHiddenInGame = false;

	SpawnTrigger->SetBoxExtent(BoxExtent);
}

// Called when the game starts or when spawned
void ABossSpawnTrigger::BeginPlay()
{
	Super::BeginPlay();
	
	SpawnTrigger->SetBoxExtent(BoxExtent);
}

// Called every frame
void ABossSpawnTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABossSpawnTrigger::OnTriggerOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AProjectBossCharacter::StaticClass()) && !m_hasSpawnedBoss)
	{
		if (!BossActor)
		{
			UE_LOG(LogTemp, Error, TEXT("No boss set!"));
			return;
		}

		// Is player
		FVector spawnLoc = IsValid(SpawnLocation) ? SpawnLocation->GetActorLocation() : FVector::ZeroVector;
		AActor* boss = GetWorld()->SpawnActor<AActor>(BossActor, spawnLoc, FRotator(), FActorSpawnParameters());
		if (boss)
		{
			UE_LOG(LogTemp, Log, TEXT("Spawned boss character at '%s'"), *boss->GetActorLocation().ToString());
			m_hasSpawnedBoss = true;
		}
	}
}
// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableDagger.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "../ProjectBossCharacter.h"

// Sets default values
AThrowableDagger::AThrowableDagger()
{
	m_damageAmount = 100.0f;
	m_speed = 1000.0f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CollisionComponent;

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AThrowableDagger::OnDaggerBeginOverlap);
	CollisionComponent->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(CollisionComponent);
}

// Called when the game starts or when spawned
void AThrowableDagger::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(5.0f);
}

// Called every frame
void AThrowableDagger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (m_movementDirection != FVector::ZeroVector)
	{
		SetActorLocation(GetActorLocation() + m_movementDirection * DeltaTime, false);
	}
}

void AThrowableDagger::SetDamage(float damage)
{
	m_damageAmount = damage;
}

void AThrowableDagger::SetMovementDirection(FVector targetVector, float speed)
{
	m_movementDirection = targetVector;
	m_movementDirection *= speed;
}

void AThrowableDagger::OnDaggerBeginOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AProjectBossCharacter::StaticClass()))
	{
		OtherActor->TakeDamage(m_damageAmount, FDamageEvent(), NULL, this);
		Destroy();
	}
}
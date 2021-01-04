// Fill out your copyright notice in the Description page of Project Settings.


#include "CapsuleAOEDamage.h"
#include "Components/CapsuleComponent.h"
#include "../Boss/BossCharacter.h"

// Sets default values
ACapsuleAOEDamage::ACapsuleAOEDamage()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	RootComponent = CapsuleComponent;

	CapsuleComponent->SetCapsuleHalfHeight(0.0f);
	CapsuleComponent->SetCapsuleRadius(0.0f);

	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ACapsuleAOEDamage::OnAOEBeginOverlap);
	
	CapsuleComponent->SetHiddenInGame(false);

	InitialLifeSpan = 1.0f;
}

// Called when the game starts or when spawned
void ACapsuleAOEDamage::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACapsuleAOEDamage::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACapsuleAOEDamage::ConfigureCapsule(float radius, float halfHeight)
{
	if (CapsuleComponent)
	{
		CapsuleComponent->SetCapsuleHalfHeight(halfHeight);
		CapsuleComponent->SetCapsuleRadius(radius);
	}
}

void ACapsuleAOEDamage::ConfigureDamage(float dmgAmount, AController* causerController, AActor* causerActor)
{
	DamageAmount = dmgAmount;
	CauserController = causerController;
	CauserActor = causerActor;
}

void ACapsuleAOEDamage::OnAOEBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACharacter::StaticClass()))
	{
		FDamageEvent dmgEvent;
		OtherActor->TakeDamage(DamageAmount, dmgEvent, CauserController, CauserActor);
	}
}


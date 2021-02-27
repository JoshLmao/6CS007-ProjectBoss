// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateCosts_PyActor.h"
#include "Kismet/GameplayStatics.h"
#include "../Boss/BossCharacter.h"
#include "../Boss/GOAP/GOAPAIController.h"

AUpdateCosts_PyActor::AUpdateCosts_PyActor()
{

}

void AUpdateCosts_PyActor::BeginPlay()
{
	Super::BeginPlay();

	// Get reference to boss in scene
	AActor* bossActor = UGameplayStatics::GetActorOfClass(GetWorld(), ABossCharacter::StaticClass());
	if (IsValid(bossActor))
	{
		m_boss = Cast<ABossCharacter>(bossActor);
	}

	// start timer
	//float seconds = 5.00f;
	//GetWorldTimerManager().SetTimer(m_updateTimerHandle, this, &AUpdateCosts_PyActor::CallPython, seconds, true, seconds);
}

void AUpdateCosts_PyActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

}

void AUpdateCosts_PyActor::CallPython()
{
	UE_LOG(LogTemp, Log, TEXT("Calling python!"));

	FString floatValue = CallPythonActorMethodString("calculate_cost", "\"ability advanced\"");
	float newCost = FCString::Atof(*floatValue);

	UE_LOG(LogTemp, Log, TEXT("Returned Value = %f"), newCost);
}

float AUpdateCosts_PyActor::GenerateCost(float baseCost, bool wasSuccess, float damage, float averageSeconds)
{

	return 0.0f;
}

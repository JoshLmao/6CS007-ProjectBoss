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
	float seconds = 5.00f;
	GetWorldTimerManager().SetTimer(m_updateTimerHandle, this, &AUpdateCosts_PyActor::CallPython, seconds, true, seconds);
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

void AUpdateCosts_PyActor::UpdateActionCost(int action, float newCost)
{
	// make sure boss is valid before executing
	if (!IsValid(m_boss)) 
	{
		UE_LOG(LogTemp, Error, TEXT("No Boss reference set!"));
		return;
	}
	
	// Access GOAP AI controller of boss
	AGOAPAIController* goapController = Cast<AGOAPAIController>(m_boss->GetController());
	if (IsValid(goapController))
	{
		// Update action index with new cost
		goapController->UpdateActionCost(action, newCost);
	}
}
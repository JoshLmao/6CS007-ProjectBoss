// Fill out your copyright notice in the Description page of Project Settings.


#include "UpdateCosts_PyActor.h"
#include "Kismet/GameplayStatics.h"
#include "../Boss/BossCharacter.h"
#include "../Boss/GOAP/GOAPAIController.h"
#include "../Helpers/CSVFileManager.h"

AUpdateCosts_PyActor::AUpdateCosts_PyActor()
{
	PredictionMultiplier = 1;

	PythonModule = "update_costs";
	PythonClass = "PBUpdateCosts";
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

	// Set prediction multiplier if one given
	if (PredictionMultiplier > 0)
	{
		CallPythonActorMethod(SET_MULTIPLIER, FString::SanitizeFloat(PredictionMultiplier));
	}

	// Init ML in Python
	FString csvPath = UCSVFileManager::GetFullFilePath();
	CallPythonActorMethod(INIT_ML, csvPath);

	/// Test to get a cost
	//float cost = GenerateCost(20.0f, true, 45.0f, 2.5436);
	//UE_LOG(LogTemp, Log, TEXT("C++ got cost at '%f'"), cost);
}

void AUpdateCosts_PyActor::Tick(float deltaTime)
{
	Super::Tick(deltaTime);

}

// Example on how to call and return value in python
void AUpdateCosts_PyActor::CallPythonExample()
{
	UE_LOG(LogTemp, Log, TEXT("Calling python!"));

	FString floatValue = CallPythonActorMethodString("calculate_cost", "\"ability advanced\"");
	float newCost = FCString::Atof(*floatValue);

	UE_LOG(LogTemp, Log, TEXT("Returned Value = %f"), newCost);
}

float AUpdateCosts_PyActor::GenerateCost(FString actionName, float baseCost, bool wasSuccess, float damage, float averageSeconds)
{
	if (baseCost < 0 || damage < 0 || averageSeconds < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid values given to GenerateCost!"));
		return 0;
	}

	if (!IsValid(this))
	{
		return 0;
	}

	// Convert success bool into float
	float successVal = wasSuccess ? 1.0f : 0.0;

	// Compile input values as arguments for format
	TArray<FStringFormatArg> args;
	args.Add(actionName);
	args.Add(FString::SanitizeFloat(baseCost));
	args.Add(FString::SanitizeFloat(successVal));
	args.Add(FString::SanitizeFloat(damage));
	args.Add(FString::SanitizeFloat(averageSeconds));
	// Format args into string
	FString arguments = FString::Format(TEXT("{0},{1},{2},{3},{4}"), FStringFormatOrderedArguments(args));

	// Call python method to create prediction and store return value
	FString predictionValue = CallPythonActorMethodString(PREDICT_COST, arguments);
	// Parse returned value into float
	float prediction = FCString::Atof(*predictionValue);

	return prediction;
}

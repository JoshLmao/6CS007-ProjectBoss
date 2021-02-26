// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CSVFileManager.generated.h"

USTRUCT()
struct FMLData
{
	GENERATED_BODY()

	/*
	*	ADDITIONAL
	*/
	// Name of the action data belongs to
	FString ActionName;

	/* 
	*	INPUTS
	*/
	// Base cost of the action
	float BaseCost;
	// Amount of attempts the action has done
	int Attempts;
	// Amount of successful attempts the action has done
	int SuccessfulAttempts;
	// Amount of damage the action deals
	float Damage;
	// Average time in seconds it took to execute the action
	float AverageExecuteSeconds;
	/*
	*	OUTPUT
	*/
	// Determined cost of the action. Used to train ML data
	float FinalCost;
};

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UCSVFileManager : public UObject
{
	GENERATED_BODY()

	/*
	*	METHODS
	*/
public:
	// Appends the ML Data to the end of each row in the CSV file.
	// Returns if function executed successfully or not
	static bool AppendData(TArray<FMLData> mlData, FString saveDirection, FString fileName);

private:
	// Appends one ML Data value to the end of a line and edits the data string, from the lastNewLineCharIndex.
	// Returns the current line's '\n' index + 1.
	static int AppendMLDataToEndOfLine(FString& dataString, float value, int lastNewLineCharIndex = -1);
};
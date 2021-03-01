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
	// Did the action succeed in it's aim
	bool DidSucceed;
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

private:
	static const FString FILE_NAME;

	/*
	*	METHODS
	*/
public:
	// Appends the ML Data to the end of each row in the CSV file.
	// Returns if function executed successfully or not
	static bool AppendData(TArray<FMLData> mlData, FString saveDirection, FString fileName);
	// Gets the full file path to the CSV file used to store ML data
	static FString GetFullFilePath();
	// Gets only the file name of the CSV file
	static FString GetFileName();
	// Gets the directory of where the CSV file will be
	static FString GetDirectory();

private:
	// Appends a "comma value" to the end of a FString, on the line number given.
	// "line=0" is reserved for header
	static void AppendMLDataToEndOfLine(TArray<FString>& dataStrings, float value, int line);

	// Gets array of headers for the ML data
	static FString GetHeader();
};

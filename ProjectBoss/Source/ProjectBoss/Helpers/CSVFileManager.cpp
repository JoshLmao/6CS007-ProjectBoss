// Fill out your copyright notice in the Description page of Project Settings.


#include "CSVFileManager.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"

/*
* CSV Data Order:
* 
* ML Inputs:
* 1. Base Cost of Action
* 2. Ability attempts
* 3. Ability successful attempts
* 4. Damage dealt
* 5. Average time (in seconds) to complete action
* 
* ML Output:
* 6. Total cost of action
*/
bool UCSVFileManager::AppendData(TArray<FMLData> mlData, FString saveDirectory, FString fileName)
{
	// Create path and check it's valid
	FString fullPath = saveDirectory + "\\" + fileName;
	FText error;
	if (!FFileHelper::IsFilenameValidForSaving(fullPath, error))
	{
		FString errorStr = error.ToString();
		UE_LOG(LogTemp, Error, TEXT("Directory '%s' isnt valid for saving! Error: '%s'"), *fullPath, *errorStr);
		return false;
	}

	FString fileString = ""; //"420,69,420\n1,2,3\n";
	char NEW_LINE = '\n';

	bool fileExists = FPlatformFileManager::Get().GetPlatformFile().FileExists(*fullPath);
	if (fileExists)
	{
		// File exists, append like normal
		bool result = FFileHelper::LoadFileToString(fileString, *fullPath);
		if (!result)
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to load existing CSV data!"));
		}
	}

	// Check that data exists inside the existing file
	bool dataExists = !fileString.IsEmpty();

	// Iterate through each data
	for (FMLData data : mlData)
	{
		// If not on first data entry, insert at the end of each line
		if (dataExists)
		{
			int eolIndex = -1;
			// INPUTS
			eolIndex = AppendMLDataToEndOfLine(fileString, data.BaseCost, eolIndex);

			eolIndex = AppendMLDataToEndOfLine(fileString, data.Attempts, eolIndex);

			eolIndex = AppendMLDataToEndOfLine(fileString, data.SuccessfulAttempts, eolIndex);

			eolIndex = AppendMLDataToEndOfLine(fileString, data.Damage, eolIndex);
				
			eolIndex = AppendMLDataToEndOfLine(fileString, data.AverageExecuteSeconds, eolIndex);
					
			// OUTPUTS
			eolIndex = AppendMLDataToEndOfLine(fileString, data.FinalCost, eolIndex);
		}
		else
		{
			// First iteration, add values normally

			// Inputs
			fileString += FString::SanitizeFloat(data.BaseCost);
			fileString += NEW_LINE;

			fileString += FString::SanitizeFloat(data.Attempts);
			fileString += NEW_LINE;

			fileString += FString::SanitizeFloat(data.SuccessfulAttempts);
			fileString += NEW_LINE;

			fileString += FString::SanitizeFloat(data.Damage);
			fileString += NEW_LINE;

			fileString += FString::SanitizeFloat(data.AverageExecuteSeconds);
			fileString += NEW_LINE;

			// Output
			fileString += FString::SanitizeFloat(data.FinalCost);
			fileString += NEW_LINE;

			// Set flag to true to append
			dataExists = true;
		}
	}

	bool result = FFileHelper::SaveStringToFile(fileString, *fullPath);
	return true;
}

int UCSVFileManager::AppendMLDataToEndOfLine(FString& dataString, float value, int lastNewLineCharIndex)
{
	// If last new line char index has been passed, start from that index
	int givenIndex = -1;
	if (lastNewLineCharIndex >= 0)
	{
		givenIndex = lastNewLineCharIndex;
	}
	
	// Find the next \n char from the given index
	int eolIndex = dataString.Find("\n", ESearchCase::Type::IgnoreCase, ESearchDir::Type::FromStart, givenIndex);
	
	// Move index to one behind \n char
	eolIndex--;
	
	// Store current value at index
	FString charAtIndex = "";
	charAtIndex += dataString[eolIndex];
	// Combine old index value with comma and new value
	FString insert = charAtIndex + "," + FString::SanitizeFloat(value);
	// Insert into string
	dataString.InsertAt(eolIndex, insert);

	// Again, find the \n char that is after the newly inserted comma and value
	eolIndex = dataString.Find("\n", ESearchCase::Type::IgnoreCase, ESearchDir::Type::FromStart, eolIndex);
	
	// Move index past that \n char and return
	eolIndex++;

	return eolIndex;
}
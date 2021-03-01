// Fill out your copyright notice in the Description page of Project Settings.


#include "CSVFileManager.h"
#include "Misc/FileHelper.h"
#include "HAL/PlatformFileManager.h"
#include "Kismet/GameplayStatics.h"

FString UCSVFileManager::GetFullFilePath()
{
	// Return both directory and file name for the full path
	return GetDirectory() + GetFileName();
}

FString UCSVFileManager::GetFileName()
{
	return "project-boss-ml-data.csv";
}

FString UCSVFileManager::GetDirectory()
{
	return UKismetSystemLibrary::GetProjectDirectory();
}

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

	TArray<FString> fileStrings;
	char NEW_LINE = '\n';

	bool fileExists = FPlatformFileManager::Get().GetPlatformFile().FileExists(*fullPath);
	if (fileExists)
	{
		// File exists, append like normal
		bool result = FFileHelper::LoadFileToStringArray(fileStrings, *fullPath);
		if (!result)
		{
			UE_LOG(LogTemp, Error, TEXT("Unable to load existing CSV data!"));
		}
	}

	// Check that data exists inside the existing file
	bool dataExists = fileStrings.Num() > 0;

	// Iterate through each data
	for (FMLData data : mlData)
	{
		// If not on first data entry, insert at the end of each line
		if (dataExists)
		{
			// Inputs
			AppendMLDataToEndOfLine(fileStrings, data.BaseCost, 1);
			AppendMLDataToEndOfLine(fileStrings, data.DidSucceed, 2);
			AppendMLDataToEndOfLine(fileStrings, data.Damage, 3);
			AppendMLDataToEndOfLine(fileStrings, data.AverageExecuteSeconds, 4);
			
			// Outputs
			AppendMLDataToEndOfLine(fileStrings, data.FinalCost, 5);
		}
		else
		{
			// First iteration, add values normally

			// Add headers to file
			FString headers = GetHeader();
			fileStrings.Add(headers);

			// Inputs
			fileStrings.Add(FString::SanitizeFloat(data.BaseCost));
			fileStrings.Add(FString::SanitizeFloat(data.DidSucceed));
			fileStrings.Add(FString::SanitizeFloat(data.Damage));
			fileStrings.Add(FString::SanitizeFloat(data.AverageExecuteSeconds));

			// Output
			fileStrings.Add(FString::SanitizeFloat(data.FinalCost));

			// Set flag to true to append
			dataExists = true;
		}
	}

	bool result = FFileHelper::SaveStringArrayToFile(fileStrings, *fullPath);
	return true;
}

void UCSVFileManager::AppendMLDataToEndOfLine(TArray<FString>& dataStrings, float value, int line)
{
	// Check line isn't header line or less than 0 (invalid)
	if (line == 0 || line < 0)
	{
		return;
	}

	// Set if line is within array bounds
	if (line < dataStrings.Num())
	{
		dataStrings[line] += "," + FString::SanitizeFloat(value);
	}
}

FString UCSVFileManager::GetHeader()
{
	// Create list of all headers
	TArray<FString> headersArr;
	// Inputs
	headersArr.Add("baseCost");
	headersArr.Add("didSucceed");
	headersArr.Add("damage");
	headersArr.Add("averageExecuteSeconds");
	// Output
	headersArr.Add("finalCost");

	// Add all together, separated with commas
	FString output = "";
	for (int i = 0; i < headersArr.Num(); i++)
	{
		output += headersArr[i];
		if (i < headersArr.Num() - 1)
		{
			output += ",";
		}
	}

	return output;
}
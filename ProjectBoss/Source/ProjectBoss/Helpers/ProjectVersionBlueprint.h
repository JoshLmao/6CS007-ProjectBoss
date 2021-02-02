// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ProjectVersionBlueprint.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UProjectVersionBlueprint : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure)
	static FString GetProjectVersion();
};

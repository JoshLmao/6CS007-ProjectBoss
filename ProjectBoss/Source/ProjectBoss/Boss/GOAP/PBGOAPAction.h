// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAPAction.h"
#include "PBGOAPAction.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UPBGOAPAction : public UGOAPAction
{
	GENERATED_BODY()

protected:
	// Creates an Atom with the given name and value
	FAtom CreateAtom(FString atmName, bool value);
	// Tries to get the targetsType actor in the scene and sets it to the target
	bool TrySetTarget(APawn* pawn);
};

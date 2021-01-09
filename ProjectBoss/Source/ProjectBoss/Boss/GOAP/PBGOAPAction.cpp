// Fill out your copyright notice in the Description page of Project Settings.


#include "PBGOAPAction.h"
#include "Kismet/KismetMathLibrary.h"

FAtom UPBGOAPAction::CreateAtom(FString atmName, bool value)
{
	FAtom atom;
	atom.name = atmName;
	atom.value = value;
	return atom;
}

bool UPBGOAPAction::TrySetTarget(APawn* pawn)
{
	if (!pawn || !targetsType)
	{
		return false;
	}

	TArray<AActor*> targets = getTargetsList(pawn);
	if (targets.Num() <= 0)
	{
		return false;
	}
	 
	for (int i = 0; i < targets.Num(); i++)
	{
		// Set target to Player
		setTarget(targets[i]);
		return true;
	}
	return false;
}

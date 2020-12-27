// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectBossGameMode.h"
#include "ProjectBossCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProjectBossGameMode::AProjectBossGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

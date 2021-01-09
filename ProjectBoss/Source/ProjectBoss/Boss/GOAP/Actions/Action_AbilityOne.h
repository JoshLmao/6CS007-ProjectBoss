// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PBGOAPAction.h"
#include "Action_AbilityOne.generated.h"

UENUM()
enum EState
{
	Enter		UMETA(DisplayName = "Enter"),
	Invisible	UMETA(DisplayName = "Invisible"),
	Exit		UMETA(DisplayName = "Exit"),
};

/**
 * 
 */
UCLASS()
class PROJECTBOSS_API UAction_AbilityOne : public UPBGOAPAction
{
	GENERATED_BODY()
	
public:
	UAction_AbilityOne();

	bool checkProceduralPrecondition(APawn* p) override;
	bool doAction(APawn* pawn) override;

private:
	EState m_currentState;
	class ABossCharacter* m_boss;

	bool UpdateFSM();
	void SetState(EState targetState);

	void BeginStart();
	void UpdateInvisible();
	void BeginExit();
};

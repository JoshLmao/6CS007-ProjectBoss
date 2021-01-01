// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Follow.h"
#include "../../../ProjectBossCharacter.h"

UAction_Follow::UAction_Follow()
{
	name = "follow";
	cost = 50.0f;
	targetsType = AProjectBossCharacter::StaticClass();

	FAtom followEffect;
	followEffect.name = "follow-player";
	followEffect.value = true;

	effects.Add(followEffect);
}
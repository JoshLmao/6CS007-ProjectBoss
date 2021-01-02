// Fill out your copyright notice in the Description page of Project Settings.


#include "Action_Wait.h"

UAction_Wait::UAction_Wait()
{
	name = "wait";
	cost = 999999.0f;
}

bool UAction_Wait::checkProceduralPrecondition(APawn* p)
{
	return true;
}

bool UAction_Wait::doAction(APawn* pawn)
{
	return false;
}

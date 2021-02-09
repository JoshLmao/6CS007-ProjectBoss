// Fill out your copyright notice in the Description page of Project Settings.


#include "UtilityHelper.h"

UtilityHelper::UtilityHelper()
{
}

UtilityHelper::~UtilityHelper()
{
}

float UtilityHelper::GetHealthDifference(float aCurrent, float aTotal, float bCurrent, float bTotal)
{
	// Update cost to be higher if the boss has more HP than the player
	float aHealthLost = aTotal - aCurrent;
	float bHealthLost = bTotal - bCurrent;
	// Health Difference will be positive if boss has less health than Player, negative if boss has more health than Player
	float healthDiff = aHealthLost - bHealthLost;

	return healthDiff;
}

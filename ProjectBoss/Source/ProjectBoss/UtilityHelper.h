// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class PROJECTBOSS_API UtilityHelper
{
public:
	UtilityHelper();
	~UtilityHelper();

	// Determines the health difference between A and B. 
	// Returns positive if B has less health than A 
	// or negative if B has more health than A
	static float GetHealthDifference(float aCurrent, float aTotal, float bCurrent, float bTotal);
};
// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectBoss : ModuleRules
{
	public ProjectBoss(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		/* Add Global Preprocessor definitions */
		// Print boss debug information to screen?
		PublicDefinitions.Add("PROJ_BOSS_SCREEN_DEBUG=1");
		// Save save machine learning data to the csv file once session is over?
		PublicDefinitions.Add("SAVE_MACHINE_LEARNING_DATA=1");
		// Should GOAP costs update to with cost related to health difference to boss and player?
		PublicDefinitions.Add("GOAP_COSTS_TO_HEALTH_DIFF=1");

		// Add dependency modules
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "AIModule", "Engine", "InputCore", "HeadMountedDisplay", "GOAP", "UMG", "NavigationSystem" });
	}
}

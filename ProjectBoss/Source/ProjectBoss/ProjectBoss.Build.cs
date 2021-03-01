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


		// Add dependency modules
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "AIModule", "Engine", "InputCore", "HeadMountedDisplay", "GOAP", "UMG", "NavigationSystem" });
	}
}

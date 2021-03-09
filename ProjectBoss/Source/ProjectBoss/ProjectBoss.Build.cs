// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectBoss : ModuleRules
{
	public ProjectBoss(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		/* Add Global Preprocessor definitions */
		// Print boss debug information to screen?
		PublicDefinitions.Add("PROJ_BOSS_SCREEN_DEBUG=0");
		// Should GOAP costs update to with cost related to health difference to boss and player?
		PublicDefinitions.Add("GOAP_COSTS_TO_HEALTH_DIFF=0");

		// Add dependency modules
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "AIModule", "Engine", "InputCore", "HeadMountedDisplay", "GOAP", "UMG", "NavigationSystem" });
	}
}

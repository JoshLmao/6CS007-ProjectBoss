// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectBoss : ModuleRules
{
	public ProjectBoss(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Add Global Preprocessor definitions
		Definitions.Add("PROJ_BOSS_SCREEN_DEBUG=1");

		// Add dependency modules
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "AIModule", "Engine", "InputCore", "HeadMountedDisplay", "GOAP", "UMG", "NavigationSystem" });
	}
}

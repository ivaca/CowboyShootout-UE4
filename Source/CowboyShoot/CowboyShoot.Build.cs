// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class CowboyShoot : ModuleRules
{
	public CowboyShoot(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}

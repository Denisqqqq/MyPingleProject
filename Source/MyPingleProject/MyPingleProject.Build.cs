// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MyPingleProject : ModuleRules
{
	public MyPingleProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}

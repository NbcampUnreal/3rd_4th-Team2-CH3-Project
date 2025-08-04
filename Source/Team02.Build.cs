// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Team02 : ModuleRules
{
	public Team02(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput",
			"AIModule",
			"NavigationSystem",
			"UMG",
			"GameplayTasks",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		
		PublicIncludePaths.AddRange(new string[] {"Team02"});
		
	}
}

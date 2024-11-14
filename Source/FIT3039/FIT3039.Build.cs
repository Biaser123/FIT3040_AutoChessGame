// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FIT3039 : ModuleRules
{
	public FIT3039(ReadOnlyTargetRules Target) : base(Target)
	{
		PrivateDependencyModuleNames.AddRange(new string[] { "ProceduralMeshComponent", "DatasmithCore",});
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "NavigationSystem", "AIModule", "Niagara", "EnhancedInput" , "UMG" , "SlateCore" ,"NavigationSystem", "GameplayTasks"});
	}
}

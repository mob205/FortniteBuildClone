// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class FortniteBuildClone : ModuleRules
{
	public FortniteBuildClone(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayAbilities", "UMG"});

		PrivateDependencyModuleNames.AddRange(new string[] { "GameplayTags", "GameplayTasks" });
		
	}
}

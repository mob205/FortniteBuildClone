using UnrealBuildTool;

public class DedicatedServers : ModuleRules
{
    public DedicatedServers(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "GameLiftServerSDK",
                "GameplayTags",
                "HTTP"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "UMG",
                "Json", "JsonUtilities"
            }
        );
    }
}
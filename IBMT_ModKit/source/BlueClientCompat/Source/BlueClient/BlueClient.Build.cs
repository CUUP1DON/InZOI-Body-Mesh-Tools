using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class BlueClient : ModuleRules
    {
        public BlueClient(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Json",
                    "JsonUtilities",
                    "AnimGraphRuntime"
                }
            );
        }
    }
}

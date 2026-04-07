using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class B1MorphCompat : ModuleRules
    {
        public B1MorphCompat(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Json",
                    "JsonUtilities"
                }
            );
        }
    }
}

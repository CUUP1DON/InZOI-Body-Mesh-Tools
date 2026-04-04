using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class BlueClientCompatEditor : ModuleRules
    {
        public BlueClientCompatEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Slate",
                    "SlateCore",
                    "UnrealEd",
                    "EditorStyle",
                    "PropertyEditor",
                    "EditorFramework",
                    "DesktopPlatform",
                    "BlueClient",
                    "Projects",
                    "WorkspaceMenuStructure"
                }
            );
        }
    }
}


using UnrealBuildTool;

namespace UnrealBuildTool.Rules
{
    public class B1MorphCompatEditor : ModuleRules
    {
        public B1MorphCompatEditor(ReadOnlyTargetRules Target) : base(Target)
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
                    "B1MorphCompat",
                    "Projects",
                    "WorkspaceMenuStructure"
                }
            );
        }
    }
}


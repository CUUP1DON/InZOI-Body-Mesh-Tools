#include "B1MorphCompatEditorModule.h"

#include "Styling/AppStyle.h"

#include "Framework/Docking/TabManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"
#include "Styling/SlateStyleRegistry.h"
#include "Widgets/Docking/SDockTab.h"

#include "SBlueClientCompatTransferWidget.h"

#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

namespace
{
const FName BlueClientCompatStyleName(TEXT("BlueClientCompatEditorStyle"));
const FName BlueClientCompatTabIconName(TEXT("BlueClientCompat.TabIcon"));
TSharedPtr<FSlateStyleSet> GBlueClientCompatStyle;

class FBlueClientCompatSlateStyle final : public FSlateStyleSet
{
public:
    explicit FBlueClientCompatSlateStyle(const FString& ResourcesDir)
        : FSlateStyleSet(BlueClientCompatStyleName)
    {
        SetParentStyleName(FAppStyle::GetAppStyleSetName());
        SetContentRoot(ResourcesDir);
        SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

        const FVector2D Icon16(16.f, 16.f);
        Set(BlueClientCompatTabIconName, new IMAGE_BRUSH(TEXT("Icon128"), Icon16));
    }
};
}

static const FName BlueClientCompatTransferTabName(TEXT("BlueClientCompatTransferTab"));

IMPLEMENT_MODULE(FBlueClientEditorModule, BlueClientEditor)

void FBlueClientEditorModule::StartupModule()
{
    if (GBlueClientCompatStyle.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*GBlueClientCompatStyle);
        GBlueClientCompatStyle.Reset();
    }

    if (TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("BlueClientCompat"));
        Plugin.IsValid())
    {
        const FString ResourcesDir = FPaths::Combine(Plugin->GetBaseDir(), TEXT("Resources"));
        const FString IconFile = FPaths::Combine(ResourcesDir, TEXT("Icon128.png"));
        if (FPaths::FileExists(IconFile))
        {
            GBlueClientCompatStyle = MakeShareable(new FBlueClientCompatSlateStyle(ResourcesDir));
            FSlateStyleRegistry::RegisterSlateStyle(*GBlueClientCompatStyle);
        }
    }

    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BlueClientCompatTransferTabName);

    FTabSpawnerEntry& Spawner = FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        BlueClientCompatTransferTabName,
        FOnSpawnTab::CreateRaw(this, &FBlueClientEditorModule::SpawnTransferTab))
        .SetDisplayName(FText::FromString(TEXT("inZOI Body Mesh Tools")))
        .SetTooltipText(FText::FromString(TEXT("B1 morph import, post-process AnimBP, and batch material slots for body skeletal meshes.")))
        .SetGroup(WorkspaceMenu::GetMenuStructure().GetToolsCategory())
        .SetMenuType(ETabSpawnerMenuType::Enabled);

    if (GBlueClientCompatStyle.IsValid())
    {
        Spawner.SetIcon(FSlateIcon(BlueClientCompatStyleName, BlueClientCompatTabIconName));
    }
}

void FBlueClientEditorModule::ShutdownModule()
{
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(BlueClientCompatTransferTabName);

    if (GBlueClientCompatStyle.IsValid())
    {
        FSlateStyleRegistry::UnRegisterSlateStyle(*GBlueClientCompatStyle);
        GBlueClientCompatStyle.Reset();
    }
}

TSharedRef<SDockTab> FBlueClientEditorModule::SpawnTransferTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SBlueClientCompatTransferWidget)
        ];
}

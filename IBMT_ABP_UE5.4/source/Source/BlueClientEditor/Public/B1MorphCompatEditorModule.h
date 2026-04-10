#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FBlueClientEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedRef<class SDockTab> SpawnTransferTab(const class FSpawnTabArgs& SpawnTabArgs);
};

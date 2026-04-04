#pragma once

#include "Modules/ModuleManager.h"

class FBlueClientModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};

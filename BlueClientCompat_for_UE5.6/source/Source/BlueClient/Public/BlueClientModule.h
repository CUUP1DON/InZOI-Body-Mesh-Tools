#pragma once

#include "Modules/ModuleManager.h"

struct IConsoleCommand;

class FBlueClientModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    IConsoleCommand* ImportConsoleCommand = nullptr;
    IConsoleCommand* SetABPConsoleCommand = nullptr;
};

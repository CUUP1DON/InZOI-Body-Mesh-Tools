#pragma once

#include "Modules/ModuleManager.h"

struct IConsoleCommand;

class FB1MorphCompatModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    IConsoleCommand* ImportConsoleCommand = nullptr;
    IConsoleCommand* SetABPConsoleCommand = nullptr;
};

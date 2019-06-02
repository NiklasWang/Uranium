#ifndef _CORE_H_
#define _CORE_H_

#include "common.h"
#include "GuiInterface.h"
#include "GuiCallback.h"
#include "Configs.h"

namespace uranium {

class Core :
    public GuiInterface,
    public GuiCallback {
public:
    int32_t start() override;
    int32_t stop() override;
    int32_t initialize() override;
    int32_t getConfig(ConfigItem key, std::string &value) override;
    int32_t getConfig(ConfigItem key, bool value) override;
    int32_t setConfig(ConfigItem key, std::string &value) override;
    int32_t setConfig(ConfigItem key, bool value) override;

public:
    int32_t appendDebugger(std::string &str) override;
    int32_t appendShell(std::string &str) override;

public:
    int32_t construct();
    int32_t destruct();
    Core(GuiCallback *gui = nullptr);
    virtual ~Core();

private:
    bool         mConstructed;
    GuiCallback *mGui;
    ModuleType   mModule;
    Configs     *mConfig;
};

}

#endif
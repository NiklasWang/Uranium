#ifndef _CORE_IMPL_H_
#define _CORE_IMPL_H_

#include "common.h"
#include "GuiInterface.h"
#include "GuiCallback.h"
#include "Configs.h"
#include "ThreadPoolEx.h"
#include "ServiceCore.h"

namespace uranium
{

class CoreImpl :
    public GuiCallback
{
public:
    int32_t start();
    int32_t stop();
    int32_t initialize();
    int32_t get(ConfigItem key, std::string &value);
    int32_t get(ConfigItem key, bool &value);
    int32_t set(ConfigItem key, std::string &value);
    int32_t set(ConfigItem key, bool value);

public:
    int32_t appendDebugger(const std::string &str) override;
    int32_t appendShell(const std::string &str) override;

public:
    int32_t construct();
    int32_t destruct();
    explicit CoreImpl(GuiCallback *gui = nullptr);
    virtual ~CoreImpl();

private:
    CoreImpl(const CoreImpl &rhs) = delete;
    CoreImpl &operator=(const CoreImpl &rhs) = delete;

private:
    bool         mConstructed;
    GuiCallback *mGui;
    ModuleType   mModule;
    Configs     *mConfig;
    ThreadPoolEx *mThreads;
    ServiceCore  *serCore;
};

}

#endif
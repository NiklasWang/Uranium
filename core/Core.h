#ifndef _CORE_H_
#define _CORE_H_

#include <string>

#include "Configs.h"
#include "GuiInterface.h"
#include "GuiCallback.h"

namespace uranium {

class CoreImpl;
class ThreadPoolEx;

class Core :
    public GuiInterface,
    public GuiCallback {
public:
    int32_t start(std::function<int32_t (int32_t)> cb =
        [](int32_t) -> int32_t { return 0; }) override;
    int32_t stop(std::function<int32_t (int32_t)> cb =
        [](int32_t) -> int32_t { return 0; }) override;
    int32_t initialize(std::function<int32_t (int32_t)> cb =
        [](int32_t) -> int32_t { return 0; }) override;
    int32_t getConfig(ConfigItem key, std::string &value) override;
    int32_t getConfig(ConfigItem key, bool &value) override;
    int32_t setConfig(ConfigItem key, std::string &value) override;
    int32_t setConfig(ConfigItem key, bool value) override;

public:
    int32_t appendDebugger(std::string str) override;
    int32_t appendShell(std::string str) override;

public:
    Core(GuiCallback *gui = nullptr);
    virtual ~Core();
    int32_t construct();
    int32_t destruct();

private:
    Core(const Core &rhs) = delete;
    Core &operator =(const Core &rhs) = delete;

private:
    bool          mConstructed;
    ModuleType    mModule;
    CoreImpl     *mImpl;
    GuiCallback  *mCb;
    ThreadPoolEx *mThreads;
};

}

#endif
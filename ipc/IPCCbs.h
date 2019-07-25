#ifndef IPC_CBS_H_
#define IPC_CBS_H_

#include "common.h"
#include "GuiCallback.h"

namespace uranium {

class IPCClient;

class IPCCbs :
    public GuiCallback  {
public:
    virtual int32_t appendDebugger(const std::string &str) override;
    virtual int32_t appendShell(const std::string &str) override;

public:
    int32_t construct();
    int32_t destruct();
    explicit IPCCbs();
    virtual ~IPCCbs();

private:
    bool          mConstructed;
    ModuleType    mModule;
    IPCClient    *mLogIPC;
    IPCClient    *mShellIPC;
};

}

#endif
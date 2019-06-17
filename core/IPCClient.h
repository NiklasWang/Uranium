#ifndef _IPC_CLIENT_H_
#define _IPC_CLIENT_H_

#include "common.h"
#include "GuiCallback.h"

namespace uranium {

class SocketClientStateMachine;

class IPCClient :
    public GuiCallback {
public:
    int32_t connect();
    int32_t sendMessage(const std::string &msg);

public:
    virtual int32_t appendDebugger(const std::string &str) override;
    virtual int32_t appendShell(const std::string &str) override;

public:
    int32_t construct();
    int32_t destruct();
    explicit IPCClient(int32_t port);
    virtual ~IPCClient();

private:
    bool          mConstructed;
    ModuleType    mModule;
    int32_t       mPort;
    bool          mAutoConnect;
    SocketClientStateMachine *mSC;
};

}

#endif
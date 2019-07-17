#ifndef _IPC_CLIENT_H_
#define _IPC_CLIENT_H_

#include <string>

#include "common.h"

namespace uranium {

class SocketClientStateMachine;

class IPCClient {
public:
    int32_t connect();
    bool    connected();
    int32_t sendMessage(const std::string &msg);

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
#ifndef _IPC_SOCKET_SERVER_H_
#define _IPC_SOCKET_SERVER_H_

#include "common.h"

namespace uranium {

class Core;
class ThreadPoolEx;
class SocketServerStateMachine;
class IPCClient;

class IPCServer {
public:
    int32_t start();
    int32_t stop();

public:
    explicit IPCServer(int32_t port);
    virtual ~IPCServer();
    int32_t construct();
    int32_t destruct();

private:
    bool          mConstructed;
    ModuleType    mModule;
    bool          mStarted;
    int32_t       mPort;
    Core         *mCore;
    ThreadPoolEx *mThreads;
    IPCClient    *mIPCClient;
    SocketServerStateMachine *mSS;
};

}

#endif
#ifndef _IPC_SOCKET_SERVER_H_
#define _IPC_SOCKET_SERVER_H_

#include <string>

#include "common.h"
#include "Semaphore.h"

namespace uranium {

class Core;
class ThreadPoolEx;
class SocketServerStateMachine;
class IPCClient;

class IPCServer {
public:
    int32_t start();
    int32_t stop();
    int32_t process();

public:
    explicit IPCServer(int32_t port);
    virtual ~IPCServer();
    int32_t construct();
    int32_t destruct();

private:
    int32_t startIPCLoop();
    int32_t stopIPCLoop();
    int32_t processIPCMessage(const char *msg);
    int32_t replyMessageCb(const std::string base, int32_t _rc);
    int32_t handleGetConfig(const char *msg);
    int32_t handleSetConfig(const char *msg);

private:
    bool          mConstructed;
    ModuleType    mModule;
    bool          mStarted;
    int32_t       mPort;
    Core         *mCore;
    ThreadPoolEx *mThreads;
    IPCClient    *mIPCClient;
    Semaphore     mExitSem;
    SocketServerStateMachine *mSS;
};

}

#endif
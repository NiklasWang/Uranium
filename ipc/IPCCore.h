#ifndef _IPC_SOCKET_CORE_H_
#define _IPC_SOCKET_CORE_H_

#include <string>

#include "common.h"
#include "Semaphore.h"

namespace uranium {

class Core;
class ThreadPoolEx;
class SocketServerStateMachine;
class IPCClient;
class IPCCbs;

class IPCCore {
public:
    int32_t start();
    int32_t stop();
    int32_t process();

public:
    int32_t updateLog(const std::string &log);
    int32_t updateShell(const std::string &shell);

public:
    explicit IPCCore(int32_t port);
    virtual ~IPCCore();
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
    IPCCbs       *mCbs;
    ThreadPoolEx *mThreads;
    IPCClient    *mIPCClient;
    Semaphore     mExitSem;
    SocketServerStateMachine *mSS;
};

}

#endif
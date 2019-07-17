#include "common.h"
#include "configuration.h"
#include "IPCInstruction.h"
#include "Core.h"
#include "Config.h"
#include "ThreadPoolEx.h"
#include "SocketServerStateMachine.h"
#include "IPCServer.h"
#include "IPCClient.h"

namespace uranium {

int32_t IPCServer::start()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mThreads->run(
            [this]() -> int32_t {
                return startIPCLoop();
            }
        );
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to run once thread, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->connect();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to connect to server:%d, %d", GUI_SOCK_PORT, rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->sendMessage(GREETING_GUI);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to greeting with gui, %d", rc);
        }
    }

    return rc;
}

int32_t IPCServer::startIPCLoop()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mSS->startServer();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to start server, %d", rc);
        } else {
            mStarted = true;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS->waitForConnect();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to wait for client connection");
        }
    }

    if (SUCCEED(rc)) {
        if (!mSS->connected()) {
            LOGD(mModule, "Client not connected, exit.");
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        do {
            char msg[SOCKET_DATA_MAX_LEN] = { '\0' };

            if (SUCCEED(rc)) {
                rc = mSS->receiveMsg(msg, sizeof(msg));
                if (rc == USER_ABORTED) {
                    LOGD(mModule, "Core exit, stop receive IPC msg.");
                } else if (FAILED(rc)) {
                    LOGE(mModule, "Failed to receive msg from socket sm, %d", rc);
                }
            }

            if (SUCCEED(rc)) {
                rc = processIPCMessage(msg);
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to handle IPC message, %d", rc);
                }
            }
        } while (rc != USER_ABORTED);
    }

    return rc;
}

int32_t IPCServer::stopIPCLoop()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        int32_t rc = mSS->cancelWaitMsg();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to cancel wait client");
        }
    }

    if (SUCCEED(rc)) {
        int32_t rc = mSS->cancelWaitConnect();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to cancel wait client");
        }
    }

    return rc;
}

int32_t IPCServer::stop()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = stopIPCLoop();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to stop IPC loop, %d", rc);
        } else {
            mStarted = false;
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->sendMessage(BYE_GUI);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to greeting with gui, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mExitSem.signal();
    }

    return rc;
}

int32_t IPCServer::process()
{
    mExitSem.wait();

    return NO_ERROR;
}

int32_t IPCServer::processIPCMessage(const char *msg)
{
    int32_t rc = NO_ERROR;
    std::string base = msg;
    std::function<int32_t (int32_t _rc)> func =
        [this, base](int32_t _rc) -> int32_t {
            return replyMessageCb(base, _rc);
        };

    if (COMPARE_SAME_STRING(msg, CORE_INIT)) {
        rc = mCore->initialize(func);
    } else if (COMPARE_SAME_STRING(msg, CORE_START)) {
        rc = mCore->start(func);
    } else if (COMPARE_SAME_STRING(msg, CORE_STOP)) {
        rc = mCore->stop(func);
    } else if (COMPARE_SAME_LEN_STRING(msg, CORE_GET_CONFIG,
        strlen(CORE_GET_CONFIG))) {
        rc = handleGetConfig(msg);
    } else if (COMPARE_SAME_LEN_STRING(msg, CORE_SET_CONFIG,
        strlen(CORE_SET_CONFIG))) {
        rc = handleSetConfig(msg);
    } else if (COMPARE_SAME_STRING(msg, CORE_EXIT)) {
        rc = stop();
    }

    if (FAILED(rc)) {
        LOGE(mModule, "Failed to handle IPC message, %s, %d", msg, rc);
    }

    return rc;
}

int32_t IPCServer::replyMessageCb(const std::string base, int32_t _rc)
{
    int32_t rc = NO_ERROR;
    std::stringstream msg;

    if (SUCCEED(rc)) {
        rc = mIPCClient->connected() ? NO_ERROR : NOT_READY;
        if (FAILED(rc)) {
            LOGE(mModule, "IPC client not constructed, can't reply now");
        }
    }

    if (SUCCEED(rc)) {
        msg << base << " " << (SUCCEED(_rc) ? REPLY_SUCCEED : REPLY_FAILED);
        rc = mIPCClient->sendMessage(msg.str());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed reply IPC, %s, %d", msg.str().c_str(), rc);
        }
    }

    return rc;
}

int32_t IPCServer::handleGetConfig(const char *msg)
{
    int32_t rc = NO_ERROR;
    std::istringstream stream(msg);
    std::string key;
    ConfigItem item;
    bool isBool;
    bool boolResult;
    std::string stringResult;
    std::stringstream reply;

    if (SUCCEED(rc)) {
        stream >> key;
        if (key != CORE_GET_CONFIG) {
            LOGE(mModule, "Wrong format, msg = %s", msg);
            rc = BAD_PROTOCAL;
        }
    }

    if (SUCCEED(rc)) {
        stream >> key;
        item = getConfigItem(key.c_str());
        if (!checkValid(item)) {
            LOGE(mModule, "Invalid config item, %s, %d",
                whoamI(item), rc);
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        switch (item) {
            case CONFIG_MASTER_MODE:
            case CONFIG_ENCRYPTION:
            case CONFIG_DEBUG_MODE:
            case CONFIG_REMOTE_SHELL: {
                isBool = true;
            } break;
            case CONFIG_USERNAME:
            case CONFIG_PASSWORD:
            case CONFIG_LOCAL_PATH:
            case CONFIG_REMOTE_PATH:
            default: {
                isBool = false;
            } break;
        }
    }

    if (SUCCEED(rc)) {
        if (isBool) {
            rc = mCore->getConfig(item, boolResult);
        } else {
            rc = mCore->getConfig(item, stringResult);
        }
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to get config %s, %d",
                whoamI(item), rc);
        }
    }

    if (SUCCEED(rc)) {
        reply << msg << " ";
        if (isBool) {
            reply << (boolResult ? BOOL_TRUE : BOOL_FALSE);
        } else {
            reply << stringResult;
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->sendMessage(reply.str());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed reply IPC, %s, %d", reply.str().c_str(), rc);
        }
    }

    return rc;
}

int32_t IPCServer::handleSetConfig(const char *msg)
{
    int32_t rc = NO_ERROR;
    std::istringstream stream(msg);
    std::string key;
    std::string value;
    ConfigItem item;
    bool isBool;

    if (SUCCEED(rc)) {
        stream >> key;
        if (key != CORE_SET_CONFIG) {
            LOGE(mModule, "Wrong format, msg = %s", msg);
            rc = BAD_PROTOCAL;
        }
    }

    if (SUCCEED(rc)) {
        stream >> key;
        item = getConfigItem(key.c_str());
        if (!checkValid(item)) {
            LOGE(mModule, "Invalid config item, %s, %d",
                whoamI(item), rc);
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        switch (item) {
            case CONFIG_MASTER_MODE:
            case CONFIG_ENCRYPTION:
            case CONFIG_DEBUG_MODE:
            case CONFIG_REMOTE_SHELL: {
                isBool = true;
            }
            case CONFIG_USERNAME:
            case CONFIG_PASSWORD:
            case CONFIG_LOCAL_PATH:
            case CONFIG_REMOTE_PATH:
            default: {
                isBool = false;
            }
        }
    }

    if (SUCCEED(rc)) {
        stream >> value;
        if (isBool) {
            rc = mCore->setConfig(item, value == BOOL_TRUE);
        } else {
            rc = mCore->setConfig(item, value);
        }
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to set config %s:%s %d",
                whoamI(item), value.c_str(), rc);
        }
    }

    return rc;
}

IPCServer::IPCServer(int32_t port) :
    mConstructed(false),
    mModule(MODULE_IPC),
    mStarted(false),
    mPort(port),
    mCore(nullptr),
    mThreads(nullptr),
    mIPCClient(nullptr),
    mSS(nullptr)
{
}

int32_t IPCServer::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        mSS = new SocketServerStateMachine(CORE_SOCK_PORT);
        if (ISNULL(mSS)) {
            LOGE(mModule, "Failed to new SocketServerStateMachine");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS->construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct SocketServerStateMachine %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mIPCClient = new IPCClient(GUI_SOCK_PORT);
        if (ISNULL(mIPCClient)) {
            LOGE(mModule, "Failed to new IPCClient");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct IPCClient %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mCore = new Core(mIPCClient);
        if (ISNULL(mCore)) {
            LOGE(mModule, "Failed to new Core");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct Core %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t IPCServer::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        if (mStarted) {
            rc = stop();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to stop IPC server, %d", rc);
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct core, %d", rc);
        } else {
            SECURE_DELETE(mCore);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSS->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct SocketServerStateMachine, %d", rc);
        } else {
            SECURE_DELETE(mSS);
        }
    }

    if (SUCCEED(rc)) {
        mThreads->removeInstance();
    }

    if (SUCCEED(rc)) {
        rc = mIPCClient->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct IPCClient, %d", rc);
        } else {
            SECURE_DELETE(mIPCClient);
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

IPCServer::~IPCServer()
{
    if (mConstructed) {
        destruct();
    }
}

}

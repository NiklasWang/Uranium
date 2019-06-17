#include "common.h"
#include "IPCInstruction.h"
#include "Core.h"
#include "ThreadPoolEx.h"
#include "SocketServerStateMachine.h"
#include "IPCServer.h"
#include "IPCClient.h"

namespace uranium {

int32_t IPCServer::start()
{
    int32_t rc = NO_ERROR;

    // TODO: to be implemented

    if (SUCCEED(rc)) {
        rc = mSS->startServer();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to start server, %d", rc);
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

int32_t IPCServer::stop()
{
    int32_t rc = NO_ERROR;
    // TODO: to be implemented
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

    // TODO: handle exit message

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
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

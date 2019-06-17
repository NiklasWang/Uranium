#include "common.h"
#include "IPCInstruction.h"
#include "configuration.h"
#include "IPCClient.h"
#include "SocketClientStateMachine.h"

namespace uranium {

int32_t IPCClient::connect()
{
    int32_t rc = mSC->connectServer();
    if (FAILED(rc)) {
        LOGE(mModule, "Failed to connect to server:%d", mPort);
    }

    return rc;
}

int32_t IPCClient::sendMessage(const std::string &msg)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (!mSC->connected() && mAutoConnect) {
            LOGD(mModule, "Didn't connect to server yet, try to connect...");
            rc = connect();
            if (FAILED(rc)) {
                mAutoConnect = false;
                LOGE(mModule, "Failed to auto connect, won't try again.");
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC->connected() ? NO_ERROR : NOT_INITED;
        if (FAILED(rc)) {
            LOGE(mModule, "Didn't connect to server:%d yet", mPort);
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC->sendMsg(msg.c_str(), msg.size());
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to send msg %s to server:%d", msg, mPort);
        }
    }

    return rc;
}

int32_t IPCClient::appendDebugger(const std::string &str)
{
    std::string msg(GUI_DEBUG " ");
    msg += str;
    return sendMessage(msg);
}

int32_t IPCClient::appendShell(const std::string &str)
{
    std::string msg(GUI_SHELL " ");
    msg += str;
    return sendMessage(msg);
}

IPCClient::IPCClient(int32_t port) :
    mConstructed(false),
    mModule(MODULE_IPC),
    mPort(port),
    mAutoConnect(true),
    mSC(nullptr)
{
}

int32_t IPCClient::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mSC = new SocketClientStateMachine(mPort);
        if (ISNULL(mSC)) {
            LOGE(mModule, "Failed to new SocketClientStateMachine");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mSC->construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to constrcut SocketClientStateMachine");
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t IPCClient::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mSC->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct SocketClientStateMachine");
        } else {
            SECURE_DELETE(mSC);
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

IPCClient::~IPCClient()
{
    if (mConstructed) {
        destruct();
    }
}

}
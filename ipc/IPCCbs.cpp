#include "IPCCbs.h"
#include "IPCClient.h"
#include "IPCInstruction.h"

namespace uranium {

int32_t IPCCbs::appendDebugger(const std::string &log)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mLogIPC->connected() ? NO_ERROR : NOT_READY;
        if (FAILED(rc)) {
            LOGE(mModule, "IPC client not constructed, can't reply now");
        }
    }

    if (SUCCEED(rc)) {
        rc = mLogIPC->sendMessage(std::string(CORE_LOG) + " " + log);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed reply IPC, %s, %d", log.c_str(), rc);
        }
    }

    return rc;
}

int32_t IPCCbs::appendShell(const std::string &shell)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = mShellIPC->connected() ? NO_ERROR : NOT_READY;
        if (FAILED(rc)) {
            LOGE(mModule, "IPC client not constructed, can't reply now");
        }
    }

    if (SUCCEED(rc)) {
        rc = mShellIPC->sendMessage(std::string(CORE_SHELL) + " " + shell);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed reply IPC, %s, %d", shell.c_str(), rc);
        }
    }

    return rc;
}

int32_t IPCCbs::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mLogIPC = new IPCClient(GUI_SOCK_PORT);
        if (ISNULL(mLogIPC)) {
            LOGE(MODULE_IPC, "Failed to new logger");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mLogIPC->construct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to construct logger %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mLogIPC->connect();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to connect logger %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mShellIPC = new IPCClient(GUI_SOCK_PORT);
        if (ISNULL(mShellIPC)) {
            LOGE(MODULE_IPC, "Failed to new shell");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = mShellIPC->construct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to construct shell %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = mShellIPC->connect();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to construct shell %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t IPCCbs::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mLogIPC->destruct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to destruct logger, %d", rc);
        } else {
            SECURE_DELETE(mLogIPC);
        }
    }

    if (SUCCEED(rc)) {
        rc = mShellIPC->destruct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to destruct shell, %d", rc);
        } else {
            SECURE_DELETE(mShellIPC);
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

IPCCbs::IPCCbs() :
    mConstructed(false),
    mModule(MODULE_IPC),
    mLogIPC(nullptr),
    mShellIPC(nullptr)
{
}

IPCCbs::~IPCCbs()
{
}

}
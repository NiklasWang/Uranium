#include "common.h"
#include "IPCInstruction.h"
#include "IPCCore.h"
#include "IPCClient.h"

namespace uranium {

IPCCore *gIpcCore = nullptr;

int32_t startIPC()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        gIpcCore = new IPCCore(CORE_SOCK_PORT);
        if (ISNULL(gIpcCore)) {
            LOGE(MODULE_IPC, "Failed to new IPCServer.");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = gIpcCore->construct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to construct IPCServer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = gIpcCore->start();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to start IPCServer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = gIpcCore->process();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to process in IPC loop, %d", rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        rc = gIpcCore->destruct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to destruct IPCServer, %d", rc);
        }
        SECURE_DELETE(gIpcCore);
    }

    return rc;
}

int32_t appendLog(const char *log)
{
    int32_t rc = NOTNULL(gIpcCore) ? NO_ERROR : NOT_READY;

    if (SUCCEED(rc)) {
        rc = gIpcCore->updateLog(log);
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to append log, %d", rc);
        }
    }

    return rc;
}

int32_t appendShell(const char *shell)
{
    int32_t rc = NOTNULL(gIpcCore) ? NO_ERROR : NOT_READY;

    if (SUCCEED(rc)) {
        rc = gIpcCore->updateShell(shell);
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to append shell, %d", rc);
        }
    }

    return rc;
}

}
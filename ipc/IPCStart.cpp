#include "common.h"
#include "IPCInstruction.h"
#include "IPCServer.h"

namespace uranium {

IPCServer *gIpcServer = nullptr;

int startIPC()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        gIpcServer = new IPCServer(CORE_SOCK_PORT);
        if (ISNULL(gIpcServer)) {
            LOGE(MODULE_IPC, "Failed to new IPCServer.");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = gIpcServer->construct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to construct IPCServer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = gIpcServer->start();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to start IPCServer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = gIpcServer->process();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to process in IPC loop, %d", rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        rc = gIpcServer->destruct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to destruct IPCServer, %d", rc);
        }
        SECURE_DELETE(gIpcServer);
    }

    return rc;
}

}
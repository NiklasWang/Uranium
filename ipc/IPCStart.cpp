#include "common.h"
#include "IPCInstruction.h"
#include "IPCServer.h"

namespace uranium {

int startIPC()
{

    int32_t rc = NO_ERROR;
    IPCServer *ipcs = nullptr;

    if (SUCCEED(rc)) {
        ipcs = new IPCServer(CORE_SOCK_PORT);
        if (ISNULL(ipcs)) {
            LOGE(MODULE_IPC, "Failed to new IPCServer.");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = ipcs->construct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to construct IPCServer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = ipcs->start();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to start IPCServer, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        rc = ipcs->process();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to process in IPC loop, %d", rc);
        }
    }

    if (SUCCEED(rc) || FAILED(rc)) {
        rc = ipcs->destruct();
        if (FAILED(rc)) {
            LOGE(MODULE_IPC, "Failed to destruct IPCServer, %d", rc);
        }
        SECURE_DELETE(ipcs);
    }

    return rc;
}

}
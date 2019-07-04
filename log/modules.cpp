#include <string.h>

#include "modules.h"

namespace uranium
{

static const char *const ModuleName[] = {
    [MODULE_OTHERS]             = "others",
    [MODULE_CORE]               = "core",
    [MODULE_CORE_IMPL]          = "core impl",
    [MODULE_GUI]                = "gui",
    [MODULE_IPC]                = "ipc",
    [MODULE_AES]                = "aes",
    [MODULE_AES_TESTER]         = "aes_tester",
    [MODULE_ENCRYPT]            = "encrypt",
    [MODULE_ENCRYPT_TESTER]     = "encrypt_tester",
    [MODULE_UUID]               = "uuid",
    [MODULE_UUID_TESTER]        = "uuid_tester",
    [MODULE_SHA1]               = "sha1",
    [MODULE_SHA1_TESTER]        = "sha1_tester",
    [MODULE_FILE]               = "file",
    [MODULE_FILE_TESTER]        = "file_tester",
    [MODULE_DIR]                = "dir",
    [MODULE_DIR_TESTER]         = "dir_tester",
    [MODULE_TRANSMITION]        = "trans",
    [MODULE_TRANSMITION_TESTER] = "trans_tester",
    [MODULE_THREAD_POOL]        = "threads",
    [MODULE_THREAD_TESTER]      = "thread_tester",
    [MODULE_MEMORY_POOL]        = "memory",
    [MODULE_MEMORY_TESTER]      = "memory_tester",
    [MODULE_SOCKET]             = "socket",
    [MODULE_SOCKET_SERVER]      = "socket_server",
    [MODULE_SOCKET_SERVER_SM]   = "socket_server_sm",
    [MODULE_SOCKET_CLIENT]      = "client",
    [MODULE_SOCKET_CLIENT_SM]   = "client_sm",
    [MODULE_OBJECT_BUFFER]      = "obj_buf",
    [MODULE_CONFIGURATION]      = "configs",
    [MODULE_UTILS]              = "utils",
    [MODULE_TOOLS]              = "tools",
    [MODULE_TESTER]             = "tester",
    [MODULE_MONITOR]            = "monitor",
    [MODULE_MONITOR_SERVER]     = "monitor_server",
    [MODULE_MAX_INVALID]        = "max_invalid",
};

static const char *const ModuleShortName[] = {
    [MODULE_OTHERS]             = "[OTHER]",
    [MODULE_CORE]               = "[ CORE]",
    [MODULE_CORE_IMPL]          = "[CIMPL]",
    [MODULE_GUI]                = "[  GUI]",
    [MODULE_IPC]                = "[  IPC]",
    [MODULE_AES]                = "[  AES]",
    [MODULE_AES_TESTER]         = "[ AEST]",
    [MODULE_ENCRYPT]            = "[ENCRY]",
    [MODULE_ENCRYPT_TESTER]     = "[ENCPT]",
    [MODULE_UUID]               = "[ UUID]",
    [MODULE_UUID_TESTER]        = "[UUIDT]",
    [MODULE_SHA1]               = "[ SHA1]",
    [MODULE_SHA1_TESTER]        = "[SHA1T]",
    [MODULE_FILE]               = "[ FILE]",
    [MODULE_FILE_TESTER]        = "[FILET]",
    [MODULE_DIR]                = "[  DIR]",
    [MODULE_DIR_TESTER]         = "[ DIRT]",
    [MODULE_TRANSMITION]        = "[TRANS]",
    [MODULE_TRANSMITION_TESTER] = "[TRANT]",
    [MODULE_THREAD_POOL]        = "[THREA]",
    [MODULE_THREAD_TESTER]      = "[THRET]",
    [MODULE_MEMORY_POOL]        = "[  MEM]",
    [MODULE_MEMORY_TESTER]      = "[ MEMT]",
    [MODULE_SOCKET]             = "[ SOCK]",
    [MODULE_SOCKET_SERVER]      = "[SOCKS]",
    [MODULE_SOCKET_SERVER_SM]   = "[ SSSM]",
    [MODULE_SOCKET_CLIENT]      = "[SOCKC]",
    [MODULE_SOCKET_CLIENT_SM]   = "[ SCSM]",
    [MODULE_OBJECT_BUFFER]      = "[OBJBF]",
    [MODULE_CONFIGURATION]      = "[ CONF]",
    [MODULE_UTILS]              = "[ UTIL]",
    [MODULE_TOOLS]              = "[ TOOL]",
    [MODULE_TESTER]             = "[TSTER]",
    [MODULE_MONITOR]            = "[MONIT]",
    [MODULE_MONITOR_SERVER]     = "[MO_SR]",
    [MODULE_MAX_INVALID]        = "[ INVA]",
};

static bool checkValid(ModuleType type)
{
    bool rc = false;

    if (type >= 0 && type < MODULE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

static ModuleType getValidType(ModuleType type)
{
    return checkValid(type) ? type : MODULE_MAX_INVALID;
}

uint32_t getMaxLenofShortName()
{
    uint32_t result = 0, len = 0;
    for (uint32_t i = 0; i <= MODULE_MAX_INVALID; i++) {
        len = strlen(ModuleShortName[i]);
        result = result < len ? len : result;
    }

    return result;
}

const char *getModuleName(ModuleType type)
{
    return ModuleName[getValidType(type)];
}

const char *getModuleShortName(ModuleType type)
{
    return ModuleShortName[getValidType(type)];
}

};


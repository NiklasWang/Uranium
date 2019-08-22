#include <stdint.h>
#include <string.h>

#include "Config.h"

namespace uranium {

const char *const gConfigName[] = {
    [CONFIG_MASTER_MODE]  = "master_slave_mode",
    [CONFIG_ENCRYPTION]   = "encryption",
    [CONFIG_DEBUG_MODE]   = "debug_mode",
    [CONFIG_REMOTE_SHELL] = "remote_shell",
    [CONFIG_USERNAME]     = "username",
    [CONFIG_PASSWORD]     = "password",
    [CONFIG_LOCAL_PATH]   = "local_path",
    [CONFIG_REMOTE_PATH]  = "remote_path",
    [CONFIG_DEBUG_SIZE]   = "debug_size",
    [CONFIG_DEBUG_FONT]   = "debug_font",
    [CONFIG_DEBUG_COLOR]  = "debug_color",
    [CONFIG_DEBUG_BG]     = "debug_bg",
    [CONFIG_SHELL_SIZE]   = "shell_size",
    [CONFIG_SHELL_FONT]   = "shell_font",
    [CONFIG_SHELL_COLOR]  = "shell_color",
    [CONFIG_SHELL_BG]     = "shell_bg",
    [CONFIG_MAX_INVALID]  = "max_invalid",
};

bool checkValid(ConfigItem item)
{
    bool rc = false;

    if (item >= 0 && item < CONFIG_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

ConfigItem getValidType(ConfigItem item)
{
    return checkValid(item) ? item : CONFIG_MAX_INVALID;
}

const char *whoamI(ConfigItem item)
{
    return gConfigName[getValidType(item)];
}

ConfigItem getConfigItem(const char *str)
{
    ConfigItem item = CONFIG_MAX_INVALID;

    for (uint32_t i = 0; i < sizeof(gConfigName); i++) {
        if (!strcmp(str, gConfigName[i])) {
            item = static_cast<ConfigItem>(i);
            break;
        }
    }

    return item;
}

}

#ifndef _CONFIG_ITEM_H_
#define _CONFIG_ITEM_H_

namespace uranium {

enum ConfigItem {
    CONFIG_MASTER_MODE,
    CONFIG_ENCRYPTION,
    CONFIG_DEBUG_MODE,
    CONFIG_REMOTE_SHELL,
    CONFIG_USERNAME,
    CONFIG_PASSWORD,
    CONFIG_LOCAL_PATH,
    CONFIG_REMOTE_PATH,
    CONFIG_DEBUG_SIZE,
    CONFIG_DEBUG_FONT,
    CONFIG_DEBUG_COLOR,
    CONFIG_DEBUG_BG,
    CONFIG_SHELL_SIZE,
    CONFIG_SHELL_FONT,
    CONFIG_SHELL_COLOR,
    CONFIG_SHELL_BG,
    CONFIG_MAX_INVALID,
};

const char *whoamI(ConfigItem item);

ConfigItem getConfigItem(const char *item);

bool checkValid(ConfigItem item);

ConfigItem getValidType(ConfigItem item);

}

#endif

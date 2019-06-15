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
    CONFIG_MAX_INVALID,
};

const char *whoamI(ConfigItem item);

ConfigItem getConfigItem(const char *item);

bool checkValid(ConfigItem item);

ConfigItem getValidType(ConfigItem item);

}

#endif

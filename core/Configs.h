#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <string>
#include <map>

#include "common.h"
#include "RWLock.h"
#include "inicpp.h"

namespace uranium {

#define CONFIG_FILE_NAME "configuration.ini"
#define INI_FILE_SECTION "Configuration"

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

class Configs {
public:

    template <typename T>
    T get(ConfigItem key);
    bool get(ConfigItem key);

    template <typename T>
    int32_t set(ConfigItem key, T value);
    int32_t set(ConfigItem key, bool value);

    int32_t load();
    int32_t save();

public:
    const char *whoamI(ConfigItem item);
    bool checkValid(ConfigItem item);
    ConfigItem getValidType(ConfigItem item);

public:
    int32_t construct();
    int32_t destruct();
    Configs();
    virtual ~Configs();

private:
    bool         mConstructed;
    ModuleType   mModule;
    std::string  mFileName;
    ini::IniFile mFile;
    std::map<std::string, std::string> mConfigs;
    static const char *const gConfigName[];
};

}

#include "Configs.hpp"

#endif
#ifndef _CONFIGS_H_
#define _CONFIGS_H_

#include <string>
#include <map>

#include "common.h"
#include "RWLock.h"
#include "inicpp.h"
#include "Config.h"

namespace uranium {

#define CONFIG_FILE_NAME "configuration.ini"
#define INI_FILE_SECTION "Configuration"

class Configs {
public:

    template <typename T>
    int32_t get(ConfigItem key, T &value);
    int32_t get(ConfigItem key, bool &value);

    template <typename T>
    int32_t set(ConfigItem key, T &value);
    int32_t set(ConfigItem key, bool &value);

    int32_t load();
    int32_t save();

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
#include <fstream>

#include "common.h"
#include "Configs.h"

namespace uranium {

const char *const Configs::gConfigName[] = {
    [CONFIG_MASTER_MODE]  = "master_slave_mode",
    [CONFIG_ENCRYPTION]   = "encryption",
    [CONFIG_DEBUG_MODE]   = "debug_mode",
    [CONFIG_REMOTE_SHELL] = "remote_shell",
    [CONFIG_USERNAME]     = "username",
    [CONFIG_PASSWORD]     = "password",
    [CONFIG_LOCAL_PATH]   = "local_path",
    [CONFIG_REMOTE_PATH]  = "remote_path",
    [CONFIG_MAX_INVALID]  = "max_invalid",
};

int32_t Configs::load()
{
    int32_t rc = NO_ERROR;

    mFile.load(mFileName.c_str());

    for (int32_t i = 0; i < CONFIG_MAX_INVALID; i++) {
        std::string key = whoamI(static_cast<ConfigItem>(i));
        std::string value = mFile[INI_FILE_SECTION][key.c_str()].as<std::string>();
        if (value.length()) {
            mConfigs[key] = value;
            LOGD(mModule, "Configuration found, %s=%s", key.c_str(), value.c_str());
        } else {
            rc |= NOT_FOUND;
            LOGE(mModule, "Not found %s in config file %s", key.c_str(), mFileName.c_str());
        }
    }

    return rc;
}

int32_t Configs::save()
{
    int32_t rc = NO_ERROR;

    for (int32_t i = 0; i < CONFIG_MAX_INVALID; i++) {
        std::string key = whoamI(static_cast<ConfigItem>(i));
        mFile[INI_FILE_SECTION][key.c_str()] = mConfigs[key];
    }

    mFile.save(mFileName.c_str());

    return rc;
}

int32_t Configs::get(ConfigItem item, bool &result)
{
    int32_t rc = NO_ERROR;
    std::string value;

    if (SUCCEED(rc)) {
        rc = get<std::string>(item, value);
        if (SUCCEED(rc)) {
            std::transform(value.begin(), value.end(), value.begin(), ::toupper);
            result = value == "TRUE";
        } else {
            LOGE(mModule, "Failed to get %s config.", whoamI(item));
        }
    }

    return NO_ERROR;
}

int32_t Configs::set(ConfigItem key, bool &value)
{
    std::string str = value ? "true" : "false";

    return set<std::string>(key, str);
}

bool Configs::checkValid(ConfigItem item)
{
    bool rc = false;

    if (item >= 0 && item < CONFIG_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

ConfigItem Configs::getValidType(ConfigItem item)
{
    return checkValid(item) ? item : CONFIG_MAX_INVALID;
}

const char *Configs::whoamI(ConfigItem item)
{
    return gConfigName[getValidType(item)];
}

int32_t Configs::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        rc = load();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to load from file %s", mFile);
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t Configs::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

Configs::Configs() :
    mConstructed(false),
    mModule(MODULE_CONFIGURATION),
    mFileName(CONFIG_FILE_NAME) {
}

Configs::~Configs()
{
    if (mConstructed) {
        destruct();
    }
}

}
#ifndef CONFIGS_HPP_
#define CONFIGS_HPP_

#include <sstream>

namespace uranium {

template <typename T>
int32_t Configs::get(ConfigItem item, T &value)
{
    int32_t rc = NO_ERROR;
    std::string key = whoamI(item);

    if (SUCCEED(rc)) {
        value = "";
        if (!checkValid(item)) {
            rc = PARAM_INVALID;
            LOGE(mModule, "Invalid config type %d", item);
        }
    }

    if (SUCCEED(rc)) {
        auto iter = mConfigs.find(key);
        if (iter == mConfigs.end()) {
            rc = NOT_FOUND;
            LOGE(mModule, "Not found config %s", key.c_str());
        }
    }

    if (SUCCEED(rc)) {
        value = mConfigs[key];
    }

    return rc;
}

template <typename T>
int32_t Configs::set(ConfigItem item, T &value)
{
    int32_t rc = NO_ERROR;
    std::string key = whoamI(item);

    if (SUCCEED(rc)) {
        if (!checkValid(item)) {
            rc = PARAM_INVALID;
            LOGE(mModule, "Invalid config type %d", item);
        }
    }

    if (SUCCEED(rc)) {
        std::stringstream ss;
        ss << value;
        mConfigs[key] = ss.str();
    }

    return rc;
}

}

#endif
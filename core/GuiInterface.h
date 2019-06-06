#ifndef _GUI_INTERFACES_H_
#define _GUI_INTERFACES_H_

#include <stdint.h>
#include <string>
#include <functional>

#include "Configs.h"

namespace uranium {

class GuiInterface {
public:
    virtual int32_t start(std::function<int32_t (int32_t)> cb =
        [](int32_t) -> int32_t { return 0; }) = 0;
    virtual int32_t stop(std::function<int32_t (int32_t)> cb =
        [](int32_t) -> int32_t { return 0; }) = 0;
    virtual int32_t initialize(std::function<int32_t (int32_t)> cb =
        [](int32_t) -> int32_t { return 0; }) = 0;

    virtual int32_t getConfig(ConfigItem key, std::string &value) = 0;
    virtual int32_t getConfig(ConfigItem key, bool &value) = 0;

    virtual int32_t setConfig(ConfigItem key, std::string &value) = 0;
    virtual int32_t setConfig(ConfigItem key, bool value) = 0;

    virtual ~GuiInterface() {}
};

}

#endif
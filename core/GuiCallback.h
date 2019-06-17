#ifndef _GUI_CALLBACKS_H_
#define _GUI_CALLBACKS_H_

#include <stdint.h>
#include <string>

namespace uranium {

class GuiCallback {
public:
    virtual int32_t appendDebugger(const std::string &str) = 0;
    virtual int32_t appendShell(const std::string &str) = 0;
    virtual ~GuiCallback() {};
};

}

#endif
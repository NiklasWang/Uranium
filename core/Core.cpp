#include "Core.h"

namespace uranium {

int32_t Core::start()
{
    sleep(1);
    LOGD(mModule, "Core start() called.");

    return NO_ERROR;
}

int32_t Core::stop()
{
    sleep(1);
    LOGD(mModule, "Core start() called.");

    return NO_ERROR;
}

int32_t Core::initialize()
{
    sleep(1);
    LOGD(mModule, "Core start() called.");

    return NO_ERROR;
}

int32_t Core::getConfig(ConfigItem key, std::string &value)
{
    return mConfig->get(key, value);
}

int32_t Core::getConfig(ConfigItem key, bool value)
{
    return mConfig->get(key, value);
}
int32_t Core::setConfig(ConfigItem key, std::string &value)
{
    return mConfig->set(key, value);
}

int32_t Core::setConfig(ConfigItem key, bool value)
{
    return mConfig->set(key, value);
}

int32_t Core::appendDebugger(std::string &str)
{
    return NOTNULL(mGui) ? mGui->appendDebugger(str) : NOT_INITED;
}

int32_t Core::appendShell(std::string &str)
{
    return NOTNULL(mGui) ? mGui->appendShell(str) : NOT_INITED;
}

int32_t Core::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mConfig = new Configs();
        rc = mConfig->construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct Configs");
        }
    }

    if (SUCCEED(rc)) {
        rc = mConfig->load();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to load configuration file");
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t Core::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mConfig->save();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to save configuration file");
        }
    }

    if (SUCCEED(rc)) {
        rc = mConfig->destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct Configs");
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

Core::Core(GuiCallback *gui) :
    mConstructed(false),
    mGui(gui),
    mModule(MODULE_CORE),
    mConfig(nullptr) {
}

Core::~Core()
{
    if (mConstructed) {
        destruct();
    }
}

}
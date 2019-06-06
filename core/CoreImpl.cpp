#include "CoreImpl.h"

namespace uranium {

int32_t CoreImpl::start()
{
    sleep(1);
    LOGD(mModule, "Core start() called.");

    return NO_ERROR;
}

int32_t CoreImpl::stop()
{
    sleep(1);
    LOGD(mModule, "Core start() called.");

    return NO_ERROR;
}

int32_t CoreImpl::initialize()
{
    sleep(1);
    LOGD(mModule, "Core start() called.");

    return NO_ERROR;
}

int32_t CoreImpl::get(ConfigItem key, std::string &value)
{
    return mConfig->get(key, value);
}

int32_t CoreImpl::get(ConfigItem key, bool &value)
{
    return mConfig->get(key, value);
}
int32_t CoreImpl::set(ConfigItem key, std::string &value)
{
    return mConfig->set(key, value);
}

int32_t CoreImpl::set(ConfigItem key, bool value)
{
    return mConfig->set(key, value);
}

int32_t CoreImpl::appendDebugger(std::string str)
{
    return NOTNULL(mGui) ? mGui->appendDebugger(str) : NOT_INITED;
}

int32_t CoreImpl::appendShell(std::string str)
{
    return NOTNULL(mGui) ? mGui->appendShell(str) : NOT_INITED;
}

int32_t CoreImpl::construct()
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

int32_t CoreImpl::destruct()
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

CoreImpl::CoreImpl(GuiCallback *gui) :
    mConstructed(false),
    mGui(gui),
    mModule(MODULE_CORE_IMPL),
    mConfig(nullptr) {
}

CoreImpl::~CoreImpl()
{
    if (mConstructed) {
        destruct();
    }
}

}
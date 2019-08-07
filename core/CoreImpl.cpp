#include "CoreImpl.h"
#include "ThreadPoolEx.h"

namespace uranium
{

int32_t CoreImpl::start()
{
    // LOGD(mModule, "Core start() called.");
    // sleep(1);
    mThreads->run(
    [this]()->int32_t {
        std::string name;
        std::string passwd;
        std::string path;
        get(CONFIG_LOCAL_PATH, path);
        get(CONFIG_USERNAME, name);
        get(CONFIG_PASSWORD, passwd);
        path = "test";
        LOGI(mModule, "LocalPath = %s", path.c_str());
        LOGI(mModule, "Username  = %s", name.c_str());
        LOGI(mModule, "Password  = %s", passwd.c_str());
        serCore = new ServiceCore(TRAN_CLINET, path, name, passwd);
        serCore->construct();
        serCore->initialize();
        serCore->start();
        return NO_ERROR;
    });
    return NO_ERROR;
}

int32_t CoreImpl::stop()
{

    mThreads->run(
    [this]()->int32_t {
        serCore->stop();
        serCore->destruct();
        delete serCore;
        serCore = nullptr;
        return NO_ERROR;
    });

    return NO_ERROR;
}

int32_t CoreImpl::initialize()
{
    LOGD(mModule, "Core initialize() called.");
    sleep(1);

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

int32_t CoreImpl::appendDebugger(const std::string &str)
{
    return NOTNULL(mGui) ? mGui->appendDebugger(str) : NOT_INITED;
}

int32_t CoreImpl::appendShell(const std::string &str)
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
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
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

    if (SUCCEED(rc)) {
        mThreads->removeInstance();
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

CoreImpl::CoreImpl(GuiCallback *gui) :
    mConstructed(false),
    mGui(gui),
    mModule(MODULE_CORE_IMPL),
    mConfig(nullptr),
    mThreads(nullptr)
{

}

CoreImpl::~CoreImpl()
{
    if (mConstructed) {
        destruct();
    }
}

}
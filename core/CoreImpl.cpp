#include "CoreImpl.h"
#include "ThreadPoolEx.h"


namespace uranium
{

int32_t CoreImpl::start()
{
    LOGD(mModule, "Core start() called.");
    
    mThreads->run(
    [this]()->int32_t {
        std::string name;
        std::string passwd;
        std::string path;
        std::string remotePaths;
        if (ISNULL(mConfig))
        {
            LOGE(mModule, "mConfig is nullptr");
            return UNKNOWN_ERROR;
        }
        mConfig->save();
        get(CONFIG_LOCAL_PATH, path);
        get(CONFIG_USERNAME, name);
        get(CONFIG_PASSWORD, passwd);
        get(CONFIG_REMOTE_PATH, remotePaths);

        // path = "test";
        remotePaths = "/home/lihb13/Templates/opt:/home/lihb13/Templates/test:/home/lihb13/Templates/good";
        LOGI(mModule, "LocalPath = %s", path.c_str());
        LOGI(mModule, "Username  = %s", name.c_str());
        LOGI(mModule, "Password  = %s", passwd.c_str());
        LOGI(mModule, "RemotePaths=%s", remotePaths.c_str());

        std::vector<std::string> remotePathVector;

        do{
            auto pos = remotePaths.find(":");
            if(pos == remotePaths.npos) {
                LOGD(mModule, "End path = %s", remotePaths.c_str());
                remotePathVector.push_back(remotePaths);
                break;
            }

            auto tmpStr = remotePaths.substr(0,pos);
            remotePathVector.push_back(tmpStr);
            LOGI(mModule, " path = %s", tmpStr.c_str());
            remotePaths = remotePaths.substr(pos+1);
            // LOGI(mModule, "reserved path = %s", remotePaths.c_str());

        }while(true);

        serCore = new ServiceCore(TRAN_CLINET, path, remotePathVector, name, passwd);
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
        // serCore->stop();
        if (NOTNULL(serCore))
        {
            serCore->stop();
            serCore->destruct();
            SECURE_DELETE(serCore);
        }

        return NO_ERROR;
    });
    LOGD(mModule, "Core stop() called.");
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

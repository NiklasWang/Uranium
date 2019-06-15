#include <QLibrary>

#include "CoreHandler.h"
#include "ui/MainWindowUi.h"

namespace uranium {

int32_t CoreHandler::construct()
{
    int32_t rc = NO_ERROR;
    typedef int32_t (*InitFunc)();

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mCore = new Core(this);
        rc = mCore->construct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct core");
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->initialize(
            [this](int32_t rc) -> int32_t {
                onInitialized(rc);
                return NO_ERROR;
            });
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to init core");
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t CoreHandler::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mCore->stop();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to stop core");
        }
    }

    if (SUCCEED(rc)) {
        rc = mCore->destruct();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct core");
        }
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t CoreHandler::start()
{
    int32_t rc = mCore->start(
        [this](int32_t rc) -> int32_t {
            onStarted(rc);
            return NO_ERROR;
        });
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to start core");
    }

    return rc;
}

int32_t CoreHandler::stop()
{
    int32_t rc = mCore->stop(
        [this](int32_t rc) -> int32_t {
            onStopped(rc);
            return NO_ERROR;
        });
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to start core");
    }

    return rc;
}

int32_t CoreHandler::getConfig(ConfigItem key, std::string &value)
{
    return mCore->getConfig(key, value);
}
int32_t CoreHandler::getConfig(ConfigItem key, bool &value)
{
    return mCore->getConfig(key, value);
}

int32_t CoreHandler::setConfig(ConfigItem key, std::string &value)
{
    int32_t rc = mCore->setConfig(key, value);
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to set string value");
    }

    return rc;
}

int32_t CoreHandler::setConfig(ConfigItem key, bool value)
{
    int32_t rc = mCore->setConfig(key, value);
    if (!SUCCEED(rc)) {
        LOGE(mModule, "Failed to set bool value");
    }

    return rc;
}

int32_t CoreHandler::onStarted(int32_t rc)
{
    if (FAILED(rc)) {
        LOGE(mModule, "Core start failed, %d", rc);
    }

    return drawUi(
        [&]() -> int32_t {
            mUi->onStarted(rc);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::onStopped(int32_t rc)
{
    if (FAILED(rc)) {
        LOGE(mModule, "Core stop failed, %d", rc);
    }

    return drawUi(
        [&]() -> int32_t {
            mUi->onStopped(rc);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::onInitialized(int32_t rc)
{
    if (FAILED(rc)) {
        LOGE(mModule, "Core initialize failed, %d", rc);
    }

    return drawUi(
        [&]() -> int32_t {
            mUi->onInitialized(rc);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::appendDebugger(std::string str)
{
    return drawUi(
        [&]() -> int32_t {
            mUi->appendDebugger(str);
            return NO_ERROR;
        }
    );
}

int32_t CoreHandler::appendShell(std::string str)
{
    return drawUi(
        [&]() -> int32_t {
            mUi->appendShell(str);
            return NO_ERROR;
        }
    );
}

CoreHandler::CoreHandler(MainWindowUi *ui) :
    mConstructed(false),
    mModule(MODULE_GUI),
    mCore(nullptr),
    mUi(ui)
{
    qRegisterMetaType<std::function<int32_t ()> >("std::function<int32_t ()>");

    connect(this, SIGNAL(drawUi(std::function<int32_t ()>)),
            this, SLOT(onDrawUi(std::function<int32_t ()>)),
            Qt::BlockingQueuedConnection);
}

CoreHandler::~CoreHandler()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t CoreHandler::onDrawUi(std::function<int32_t ()> func)
{
    return func();
}

}

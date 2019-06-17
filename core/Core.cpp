#include "common.h"
#include "Core.h"
#include "CoreImpl.h"
#include "ThreadPoolEx.h"

namespace uranium {

#define CONSTRUCT_IMPL_ONCE()                                    \
    ({                                                           \
        int32_t __rc = NO_ERROR;                                 \
        if (ISNULL(mImpl)) {                                     \
            mImpl = new CoreImpl(mCb);                           \
            if (ISNULL(mImpl)) {                                 \
                __rc = NO_MEMORY;                                \
                LOGE(mModule, "Failed to create core impl.");    \
            }                                                    \
        }                                                        \
        if (NOTNULL(mImpl)) {                                    \
            __rc = mImpl->construct();                            \
            if (FAILED(__rc)) {                                  \
                LOGE(mModule, "Failed to construct core impl."); \
            }                                                    \
        }                                                        \
        __rc;                                                    \
    })

int32_t Core::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        rc = CONSTRUCT_IMPL_ONCE();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to construct core impl");
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

int32_t Core::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mImpl->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct core impl");
        } else {
            SECURE_DELETE(mImpl);
        }
    }

    if (SUCCEED(rc)) {
        mThreads->removeInstance();
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t Core::start(std::function<int32_t (int32_t)> cb)
{
    return mThreads->run(
        [this, cb]() -> int32_t {
            int32_t _rc = NO_ERROR;
            _rc = mImpl->start();
            cb(_rc);
            if (FAILED(_rc)) {
                LOGE(mModule, "Failed to %s core impl, %d",
                    __FUNCTION__, _rc);
            }
            return _rc;
        }
    );
}

int32_t Core::stop(std::function<int32_t (int32_t)> cb)
{
    return mThreads->run(
        [this, cb]() -> int32_t {
            int32_t _rc = NO_ERROR;
            _rc = mImpl->stop();
            cb(_rc);
            if (FAILED(_rc)) {
                LOGE(mModule, "Failed to %s core impl, %d",
                    __FUNCTION__, _rc);
            }
            return _rc;
        }
    );
}

int32_t Core::initialize(std::function<int32_t (int32_t)> cb)
{
    return mThreads->run(
        [this, cb]() -> int32_t {
            int32_t _rc = NO_ERROR;
            _rc = mImpl->initialize();
            cb(_rc);
            if (FAILED(_rc)) {
                LOGE(mModule, "Failed to %s core impl, %d",
                    __FUNCTION__, _rc);
            }
            return _rc;
        }
    );
}

int32_t Core::getConfig(ConfigItem key, std::string &value)
{
    return mImpl->get(key, value);
}

int32_t Core::getConfig(ConfigItem key, bool &value)
{
    return mImpl->get(key, value);
}

int32_t Core::setConfig(ConfigItem key, std::string &value)
{
    return mImpl->set(key, value);
}

int32_t Core::setConfig(ConfigItem key, bool value)
{
    return mImpl->set(key, value);
}

int32_t Core::appendDebugger(const std::string &str)
{
    return mThreads->run(
        [this, str]() -> int32_t {
            int32_t _rc = NO_ERROR;
            _rc = mCb->appendDebugger(str);
            if (FAILED(_rc)) {
                LOGE(mModule, "Failed to %s to gui, %d",
                    __FUNCTION__, _rc);
            }
            return _rc;
        }
    );
}

int32_t Core::appendShell(const std::string &str)
{
    return mThreads->run(
        [this, str]() -> int32_t {
            int32_t _rc = NO_ERROR;
            _rc = mCb->appendShell(str);
            if (FAILED(_rc)) {
                LOGE(mModule, "Failed to %s to gui, %d",
                    __FUNCTION__, _rc);
            }
            return _rc;
        }
    );
}

Core::Core(GuiCallback *gui) :
    mConstructed(false),
    mModule(MODULE_CORE),
    mImpl(nullptr),
    mCb(gui),
    mThreads(nullptr)
{
}

Core::~Core()
{
    CoreImpl *impl = mImpl;
    mImpl = nullptr;
    SECURE_DELETE(impl);
}

}

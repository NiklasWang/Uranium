#include "EncryptCore.h"


namespace uranium
{

int32_t EncryptCore::generateDictionKeys(void)
{
    mDiction->generateKeys();
    return NO_ERROR;
}

int32_t EncryptCore::sotraDiction(const std::string  &filePath, std::function<int32_t ()>cb)
{
    return mThreads->run(
    [ = ]()-> int32_t {
        int32_t __rc = NO_ERROR;
        __rc = mDiction->sotraDiction(filePath);
        cb();
        return __rc;
    });
}

int32_t EncryptCore::loadDiction(const std::string &filePath)
{
    return mThreads->run(
    [ = ]()-> int32_t {
        return mDiction->loadDiction(filePath);
    });
}

int32_t EncryptCore::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        mDiction = new Dictionary();
        if (ISNULL(mDiction)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
        }
        rc = mDiction->construct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed construct mDiction\n");
        }
    }
    return rc;
}

int32_t EncryptCore::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mDiction->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed ServiceCore destruct mTransCore\n");
        }
        SECURE_DELETE(mDiction);
    }

    return rc;
}

EncryptCore::EncryptCore():
    mConstructed(false),
    mThreads(NULL),
    mDiction(NULL),
    mEncryMan(NULL)
{

}
EncryptCore::~EncryptCore()
{
    mConstructed = false;
}


}
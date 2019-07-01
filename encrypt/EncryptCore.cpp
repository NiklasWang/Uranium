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
        mSyncFlage = true;
        return __rc;
    });
}

int32_t EncryptCore::loadDiction(const std::string &filePath)
{
    return mThreads->run(
    [ = ]()-> int32_t {
        mDiction->loadDiction(filePath);
        mSyncFlage = true;
        return NO_ERROR;
    });
}

void EncryptCore::setDynamicEnable(void)
{
    return mDiction->setDynamicEnable();
}

void EncryptCore::setDynamicDisable(void)
{
    return mDiction->setDynamicDisable();
}

void EncryptCore::getKeys(const struct timeval &timeValue, uint8_t *key)
{
    return mDiction->getKeys(timeValue, (char *)key);
}

int32_t EncryptCore::encryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16)
{
    return mEncryMan->encryptStream(origFile, destFile, key16);
}

int32_t EncryptCore::decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16)
{
    int32_t rc = NO_ERROR;
    uint32_t origChecksum[4], destChecksum[4];

    if (SUCCEED(rc)) {
        memset(origChecksum, 0, sizeof(origChecksum));
        memset(destChecksum, 0, sizeof(destChecksum));
    }
    if (SUCCEED(rc)) {
        rc = mEncryMan->decryptStream(origFile, destFile, key16, origChecksum, destChecksum);
        if (FAILED(rc)) {
            LOGE(mModule, "decryptStream failed\n");
        }
    }
    if (SUCCEED(rc)) {
        for (int i = 0; i < 4; i++) {
            if (origChecksum[i] != destChecksum[i]) {
                rc = NOT_INITED;
                LOGE(mModule, "Checksum compare failed\n");
                break;
            }
        }
    }
    return rc;
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

    if (SUCCEED(rc)) {
        mEncryMan = new EncryptFile();
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

    if (SUCCEED(rc)) {
        SECURE_DELETE(mEncryMan);
    }
    return rc;
}

EncryptCore::EncryptCore():
    mConstructed(false),
    mSyncFlage(false),
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
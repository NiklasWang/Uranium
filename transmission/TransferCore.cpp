#include "TransferCore.h"

namespace uranium
{

static const uint8_t defaultPublicKeys[16] = {
    0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xBA,
    0x34, 0x56, 0x78, 0xBa, 0xbc, 0xde, 0xf7, 0x12
};

int32_t TransferCore::send(const std::string path)
{
    return mThreads->run(
    [this, path]()->int32_t {
        // int32_t __rc = NO_ERROR;
        TRANSFER_BUFFER_T *pTranBuffer = mTransMang->createTransferBuffer();
        pTranBuffer->mode = TRAN_MODE_FEX;
        std::string encryPath = path + ".encry";
        std::string sendPath;

        /* ecry path */
        if (NOTNULL(mEncrypt))
        {
            mEncrypt->encryptStream(path, encryPath, defaultPublicKeys);
            sendPath = encryPath;
        } else
        {
            sendPath = path;
            LOGD(mModule, "Encrypt manager not support\n");
        }

        pTranBuffer->buffer = (void*) sendPath.c_str();
        mTransMang->pushData(*pTranBuffer);
        mTransMang->destoryTransferBuffer(pTranBuffer);
        return NO_ERROR;
        // return __rc;
    });
}

int32_t TransferCore::receive(std::function<int32_t (std::string &filePath)> cb)
{
    return mThreads->run(
    [this, cb]()->int32_t {
        int32_t __rc = NO_ERROR;
        char buffer[1024];
        std::string mPath;
        std::string destPath;
        if (mRuning)
        {
            LOGE(mModule, "Receive has already runing ...");
            return NOT_INITED;
        } else
        {
            mRuning  = true;
        }

        TRANSFER_BUFFER_T *pTranBuffer = mTransMang->createTransferBuffer(TRAN_MODE_FEX, buffer, 1024);
        do
        {
            if (!mRuning) {
                break;
            }
            sleep(3);
            __rc = mTransMang->pullData(*pTranBuffer);
            if (SUCCEED(__rc)) {
                mPath = buffer;
                destPath = mPath;
                if (NOTNULL(mEncrypt)) {
                    destPath += ".decry";
                    mEncrypt->decryptStream(mPath, destPath, defaultPublicKeys);
                }
                if (NOTNULL(cb)) {
                    cb(destPath);
                } else {
                    LOGE(mModule, "Call back funtion is not exit");
                }
            }
        } while (mRuning);
        mTransMang->destoryTransferBuffer(pTranBuffer);
        return NO_ERROR;
    });
}


int32_t TransferCore::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        LOGE(mModule, "TransferCore construct has already inited");
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
        mTransMang = mTranFact->createTransferObject(TRAN_MODE_FEX, mTranStatus, mName, mPassWd);
        if (NOTNULL(mTransMang)) {
            rc = mTransMang->construct();
            if (FAILED(rc)) {
                LOGE(mModule, "construct  return = %d\n", rc);
            }

        }
    }
    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t TransferCore::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        LOGE(mModule, "Not inited");
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    mRuning = false;
    sleep(4);
    mEncrypt = NULL;

    if (NOTNULL(mTransMang)) {
        rc = mTransMang->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "construct  return = %d\n", rc);
        }
        SECURE_DELETE(mTransMang);
    }



    if (NOTNULL(mThreads)) {
        mThreads->removeInstance();
        mThreads = NULL;
    }

    return rc;
}

TransferCore::TransferCore(TRANSFER_STATUS_ENUM tranStatus, EncryptCore *encrypt, std::string name, std::string passwd):
    mRuning(false),
    mConstructed(false),
    mModule(MODULE_TRANSMITION),
    mTransMang(NULL),
    mThreads(NULL),
    mEncrypt(encrypt),
    mTranStatus(tranStatus),
    mName(name),
    mPassWd(passwd)
{
    mTranFact = TransferFactory::create();
}

TransferCore::~TransferCore()
{
    SECURE_DELETE(mTranFact);
}

}
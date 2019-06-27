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
        /* ecry path */
        std::cout << "start 111 ------------" << path << encryPath << "\n";
        mEncrypt->encryptStream(path, encryPath, defaultPublicKeys);

        pTranBuffer->buffer = (void*) encryPath.c_str();
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
        TRANSFER_BUFFER_T *pTranBuffer = mTransMang->createTransferBuffer(TRAN_MODE_FEX, buffer, 1024);
        do
        {
            sleep(5);

            __rc = mTransMang->pullData(*pTranBuffer);
            if (SUCCEED(__rc)) {
                mPath = buffer;
                destPath = mPath + ".decry";
                std::cout << "start 222 ------------\n";
                mEncrypt->decryptStream(mPath, destPath, defaultPublicKeys);
                cb(destPath);
            }
        } while (true);
        mTransMang->destoryTransferBuffer(pTranBuffer);
        return NO_ERROR;
    });
}


int32_t TransferCore::construct()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        mTransMang = mTranFact->createTransferObject(TRAN_MODE_FEX, mTranStatus);
#if 0
        if (NOTNULL(mTransMang)) {
            rc = mTransMang->
        }
#endif
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
        }
    }

    return rc;
}

int32_t TransferCore::destruct()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        SECURE_DELETE(mTransMang);
    }

    {
        mThreads->removeInstance();
        mThreads = NULL;
    }

    return rc;
}

TransferCore::TransferCore(TRANSFER_STATUS_ENUM tranStatus, EncryptCore *encrypt):
    mTransMang(NULL),
    mThreads(NULL),
    mEncrypt(encrypt),
    mTranStatus(tranStatus)
    // mModule(0),
{
    mTranFact = TransferFactory::create();
}

TransferCore::~TransferCore()
{
    //SECURE_DELETE(mTransMang);
    // SECURE_DELETE(mThreads);
}

}
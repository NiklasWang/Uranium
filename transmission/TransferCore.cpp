#include "TransferCore.h"

namespace uranium
{

int32_t TransferCore::send(const std::string path)
{
    return mThreads->run(
    [this, path]()->int32_t {
        // int32_t __rc = NO_ERROR;
        TRANSFER_BUFFER_T *pTranBuffer = mTransMang->createTransferBuffer();
        pTranBuffer->mode = TRAN_MODE_FEX;
        pTranBuffer->buffer = (void*) path.c_str();
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
        TRANSFER_BUFFER_T *pTranBuffer = mTransMang->createTransferBuffer(TRAN_MODE_FEX, buffer, 1024);
        do
        {
            sleep(5);
            __rc = mTransMang->pullData(*pTranBuffer);
            if (SUCCEED(__rc)) {
                mPath = buffer;
                cb(mPath);
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

TransferCore::TransferCore(TRANSFER_STATUS_ENUM tranStatus):
    mTransMang(NULL),
    mThreads(NULL),
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
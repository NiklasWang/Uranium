/**
 * @file TransferInterface.cpp
 * @brief
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-05-31
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */

#include "common.h"
#include "TransferInterface.h"

namespace uranium
{

TransferManager::TransferManager():
    mModule(MODULE_ENCRYPT)
{

}

TransferManager::~TransferManager()
{

}

TRANSFER_BUFFER_T* TransferManager::createTransferBuffer(void)
{
    TRANSFER_BUFFER_T *pTransBuf = new TRANSFER_BUFFER_T;

    if (ISNULL(pTransBuf)) {
        LOGE(mModule, "create transfer buffer failed! <out of memory> \n");
    }
    return pTransBuf;
}

TRANSFER_BUFFER_T* TransferManager::createTransferBuffer(TRANSFER_MODE_E mode,  void *pBuffer, uint32_t length)
{
    int32_t rc = 0;
    TRANSFER_BUFFER_T *pTransBuf = NULL;
    if (SUCCEED(rc)) {
        pTransBuf = createTransferBuffer();
        if (ISNULL(pTransBuf)) {
            rc = -1;
        }
    }

    if (SUCCEED(rc)) {
        pTransBuf->mode = mode;
        pTransBuf->buffer = pBuffer;
        pTransBuf->length = length;
    }

    return pTransBuf;

}

void TransferManager::destoryTransferBuffer(const TRANSFER_BUFFER_T *pTransBuf)
{
    SECURE_DELETE(pTransBuf);
}

};

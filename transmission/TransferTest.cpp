/**
 * @file main.cpp
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
#include "FexTransfer.h"
#include "TransferFactory.h"


namespace  uranium
{

int __TransferMain(int argc, char **argv)
{
    TransferFactory *pFactory = TransferFactory::create();
    TRANSFER_BUFFER_T tranBuffer;
    TransferManager   *pTransManage = NULL;
    int32_t rc = 0;

    if (!ISNULL(pFactory)) {
        pTransManage = pFactory->createTransferObject(TRAN_MODE_FEX);
        if (ISNULL(pTransManage)) {
            printf("create failed!\n");
        }
    }

    if (SUCCEED(rc)) {
        tranBuffer.mode = TRAN_MODE_FEX;
        tranBuffer.buffer = malloc(1024);
        tranBuffer.length = 1024;
        sprintf((char *)tranBuffer.buffer, "fex -l > /tmp/lhb.bin");
        pTransManage->pullData(tranBuffer);
        memset(tranBuffer.buffer, 0, tranBuffer.length);
        sprintf((char *)tranBuffer.buffer, "fex -u /tmp/transfer_test");
        pTransManage->pushData(tranBuffer);
    }

    free(tranBuffer.buffer);
    return rc;
}

};


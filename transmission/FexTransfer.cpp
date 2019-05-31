/**
 * @file FexTransfer.cpp
 * @brief fex transfer function
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-05-31
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */

#include <stdlib.h>

#include "common.h"
#include "TransferInterface.h"
#include "FexTransfer.h"

namespace uranium
{

FexTransfer::FexTransfer():
    mModule(MODULE_ENCRYPT)
{

}

FexTransfer::~FexTransfer()
{

}

uint32_t FexTransfer::pushData(TRANSFER_BUFFER_T &cmd)
{
    int32_t rc = 0;

    if (SUCCEED(rc)) {
        if (cmd.mode != TRAN_MODE_FEX) {
            rc = -1;
            LOGE(mModule, "Mode mismatching!");
        }
    }

    if (SUCCEED(rc)) {
        if (!ISNULL(cmd.buffer)) {
            rc = system((const char *)cmd.buffer);
            if (FAILED(rc)) {
                LOGE(mModule, "Runing cmd=%s failed!", cmd.buffer);
            }
        }
    }

    return rc;
}

uint32_t FexTransfer::pullData(TRANSFER_BUFFER_T &cmd)
{
    return pushData(cmd);
}

};

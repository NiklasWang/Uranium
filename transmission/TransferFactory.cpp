/**
 * @file TransferFactory.cpp
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

namespace uranium
{

TransferFactory* TransferFactory::create()
{
    TransferFactory *pFactory = new TransferFactory;
    return pFactory;
}

void TransferFactory::destory()
{
    delete this;
}

TransferManager* TransferFactory::createTransferObject(const TRANSFER_MODE_ENUM transModes)
{
    TransferManager *pManager = NULL;
    switch (transModes) {
        case TRAN_MODE_FEX:
            pManager = new FexTransfer;
            break;
        default:
            pManager = NULL;
            break;

    }
    return pManager;
}

TransferFactory::~TransferFactory()
{

}

};

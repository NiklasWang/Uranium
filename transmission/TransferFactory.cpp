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

TransferManager* TransferFactory::createTransferObject(const TRANSFER_MODE_ENUM transModes, TRANSFER_STATUS_E tranDirct)
{
    TransferManager *pManager = NULL;
    switch (transModes) {
        case TRAN_MODE_FEX:
            pManager = new FexTransfer(tranDirct);
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

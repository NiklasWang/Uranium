/**
 * @file FexTransfer.h
 * @brief  transfer data thorugh fex commond
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-05-30
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */
#ifndef __FEXTRANSFER_H__
#define __FEXTRANSFER_H__

#include "TransferInterface.h"

namespace uranium
{

class FexTransfer : public TransferManager
{
public:
    FexTransfer();
    ~FexTransfer();
    virtual uint32_t pushData(TRANSFER_BUFFER_T &cmd);
    virtual uint32_t pullData(TRANSFER_BUFFER_T &cmd);
private:
    ModuleType      mModule;
};

};

#endif //__FEXTRANSFER_H__


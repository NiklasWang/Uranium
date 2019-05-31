/**
 * @file TransferInterface.hpp
 * @brief  file transfer interface function
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-05-30
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */

#ifndef __TRANSFERINTERFACE_H__
#define __TRANSFERINTERFACE_H__

namespace uranium
{

typedef enum TRANSFER_MODE_ENUM {
    TRAN_MODE_FEX,
    TRAN_MODE_SOCKET
} TRANSFER_MODE_E;

typedef enum TRANSFER_STATUS_ENUM {
    TRAN_CLINET,
    TRANS_SERVER
} TRANSFER_STATUS_E;

typedef struct  TRANSFER_BUFFER_TAG {
    TRANSFER_MODE_E mode;
    void         *buffer;
    uint32_t        length;
} TRANSFER_BUFFER_T;

class TransferManager
{
public:
    TransferManager();
    ~TransferManager();
    virtual uint32_t pushData(TRANSFER_BUFFER_T &cmd) = 0;
    virtual uint32_t pullData(TRANSFER_BUFFER_T &cmd) = 0;
    TRANSFER_BUFFER_T* createTransferBuffer(void);
    TRANSFER_BUFFER_T* createTransferBuffer(TRANSFER_MODE_E mode,  void *pBuffer, uint32_t length);
    void destoryTransferBuffer(const TRANSFER_BUFFER_T* pTransBuf);
private:
    ModuleType          mModule;
    TRANSFER_STATUS_E   mTranDirct;
};

extern int __TransferMain(int argc, char **argv);

}; /* namespace uranium */
#endif //__TRANSFERINTERFACE_H__

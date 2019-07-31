#ifndef __TRANSFER_CORE_H__
#define __TRANSFER_CORE_H__

#include "common.h"
#include "FexTransfer.h"
#include "threads/ThreadPoolEx.h"
#include "TransferInterface.h"
#include "TransferFactory.h"
#include "EncryptCore.h"

namespace uranium
{

class TransferFactory;
class EncryptCore;
class TransferCore
{
public:
    int32_t send(const std::string path);
    int32_t receive(std::function<int32_t (std::string &filePath)> cb);

public:
    int32_t construct();
    int32_t destruct();
    TransferCore(TRANSFER_STATUS_ENUM tranStatus, EncryptCore *encrypt, std::string name, std::string passwd);
    virtual ~TransferCore();

private:
    TransferCore() = delete;
    TransferCore(const TransferCore &rhs) = delete;
    TransferCore &operator=(const TransferCore &rhs) = delete;

private:
    ModuleType      mModule;
    TransferManager *mTransMang;
    ThreadPoolEx    *mThreads;
    TransferFactory *mTranFact;
    EncryptCore     *mEncrypt;
    TRANSFER_STATUS_ENUM    mTranStatus;
    std::string     mName;
    std::string     mPassWd;
};

}

#endif //__TRANSFER_CORE_H__
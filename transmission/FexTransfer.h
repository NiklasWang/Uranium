#ifndef __FEXTRANSFER_H__
#define __FEXTRANSFER_H__

#include "TransferInterface.h"
#include <iostream>
#include <stdio.h>

namespace uranium
{

class FexTransfer : public TransferManager
{
public:
    FexTransfer(TRANSFER_STATUS_E tranDirct);
    ~FexTransfer();
    virtual uint32_t pushData(TRANSFER_BUFFER_T &cmd);
    virtual uint32_t pullData(TRANSFER_BUFFER_T &cmd);
private:
    ModuleType          mModule;
    TRANSFER_STATUS_E   mTranDirct;
    std::string         mFileLastTime;
    uint32_t readFile(const std::string &filePath, uint8_t *pBuffer, uint32_t &length);
    uint32_t writeFile(const std::string &filePath, const uint8_t *buffer, uint32_t length);
    time_t str_to_time_t(const std::string& ATime, const std::string& AFormat = "%d-%d-%d %d:%d:%d");
};

};

#endif //__FEXTRANSFER_H__


#ifndef __FEXTRANSFER_H__
#define __FEXTRANSFER_H__

#include "TransferInterface.h"
#include <iostream>
#include <stdio.h>

namespace uranium
{

#define CLINETFILE          ("CLI_FILE")
#define SERVERFILE          ("SER_FILE")
#define WORK_DIRPATH        ("/tmp/Uranium/")
#define CLINET_PATH         ("Clinet/")
#define SERVER_PATH         ("Server/")
#define DICTIONARY_NAME     ("direction.bin")
#define FILE_INFOS_NAME     ("fileInfos.bin")
#define TAR_MODIR_NAME      ("modir.tar.bz2")
#define TRA_SYNC_FILE_NAME  ("MO_SYNC_FILE.bin")

#define FILE_BUFFER_SIZE    (1024*1024)

class FexTransfer : public TransferManager
{
public:
    FexTransfer(TRANSFER_STATUS_E tranDirct);
    virtual ~FexTransfer();
    virtual uint32_t pushData(TRANSFER_BUFFER_T &cmd);
    virtual uint32_t pullData(TRANSFER_BUFFER_T &cmd);

private:
    uint32_t readFile(const std::string &filePath, uint8_t *pBuffer, uint32_t &length);
    uint32_t writeFile(const std::string &filePath, const uint8_t *buffer, uint32_t length);
    time_t str_to_time_t(const std::string& ATime, const std::string& AFormat = "%d-%d-%d %d:%d:%d");

private:
    ModuleType          mModule;
    TRANSFER_STATUS_E   mTranDirct;
    std::string         mFileLastTime;
    pthread_mutex_t     mTransMutex;

};

};

#endif //__FEXTRANSFER_H__


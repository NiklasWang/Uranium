#ifndef __SERVICECORE_H__
#define __SERVICECORE_H__

#include <string>
#include <functional>
#include <dirent.h>
#include <sys/types.h>

#include "common.h"
#include "threads/ThreadPoolEx.h"
//#include "transmission/TransferInterface.h"
#include "transmission/TransferCore.h"
#include "monitor/MonitorCore.h"
#include "SemaphoreTimeout.h"
#include "encrypt/EncryptCore.h"

namespace uranium
{

#define EVENT_FLAGE_MASK        (0x4954)
#define MOENTRY_FLAGE_MASK      (0x4953)
#define DIR_FILE_NAME           ("dirCMD")

typedef enum  TREANFER_EVENT_ENUM {
    SEND_EMPTY = 0xA,
    CONFIG_EVT,
    CONFIG_ACK,
    DIR_MO_EVT,
    DIR_MO_ACK
} TREANFER_EVENT_E;

typedef enum DIR_MO_EVT_ENUM {
    DIR_MO_EVT_LOAD_MD5INFOS = 0xB0,
    DIR_MO_EVT_STORA_MD5INFOS,
    DIR_MO_EVT_LOAD,
    DIR_MO_EVT_STORA,
    DIR_MO_EVT_MODATA
} DIR_MO_EVT_E;

typedef enum DIR_MO_ACK_ENUM {
    DIR_MO_ACK_OK = 0,
    DIR_MO_ACK_EMPTY,
} DIR_MO_ACK_E;

typedef enum CONFIG_EVT_ENUM {
    CONFIG_DIC_LOAD = 0xA0,
    CONFIG_DIC_STORA,
    CONFIG_FILE_TRAN,
    CONFIG_FILE_STORA,
    CONFIG_READY_STATUS,
    CONFIG_READY_OK
    // CONFIG_DIR_STORAGE,
} CONFIG_EVT_E;

typedef struct TRANSFER_HEADE_TAG {
    uint32_t         flages;
    TREANFER_EVENT_E evtKey;
    uint32_t         evtValue;
} TRAN_HEADE_T;

typedef struct TRANSFER_ENTRY_FILE_TAG {
    uint32_t        flages;
    char            fileName[128];
    uint32_t        value;
    uint32_t        fileSize;
    // uint32_t        offset;
} TRANSFER_ENTRY_FILE_T;

#if 0
// class TransferCore;
#endif
class ServiceCore
{
public:
    int32_t start();
    int32_t stop();
    int32_t initialize();

private:
    int32_t serverInitialize();
    int32_t clientInitialize();

public:
    ServiceCore(TRANSFER_STATUS_ENUM  tranStatus, const std::string localPath);
    virtual~ServiceCore();
    int32_t construct();
    int32_t destruct();

private:
    int32_t tarnsferServer2Clinet(void);
    int32_t praseStora2Local(const std::string &filePath);
    int32_t transferLoadFileInfos();
    int32_t transferStoraFilelInfos(const std::string &filePath);
    // int32_t transferStoraFilelInfos(const std::string &filePath)
    int32_t createEntryFile(const std::string &fileName, uint32_t value, bool fistFlage);
    int32_t praseEntryFile(const std::string& inPath);
    int32_t monitorDirCallBack(std::map<std::string, uint32_t>&diffFile);

private:
    int32_t doHandleMoEvt(const TRAN_HEADE_T& traHead, const std::string &filePath);
    int32_t doHandleMoAck(const TRAN_HEADE_T& traHead);
    int32_t doHandleConEvt(const TRAN_HEADE_T& traHead, const std::string &filePath);
    int32_t doHandleConAck(const TRAN_HEADE_T& traHead);

private:
    int32_t transferConfigs(void);
    int32_t transferDictionaryCMD(TREANFER_EVENT_ENUM evt, uint32_t cmdValue, bool sendOn);
    int32_t transferAppendData(const std::string &filePath);
    int32_t transferAppendData(const std::string &inFilePath, const std::string &outFilePath);
    int32_t reduceTranHeaderData(const std::string &inPath, const std::string &outPath);
    std::string appendBasePath(const std::string dirPath);
    int32_t transferCompleteWorks(void);
    int32_t transferModify(const std::string& inPath);
    int32_t lisnenReceiveHandler(std::string &filePath);
    TRAN_HEADE_T *createTranHeade(void);
    int32_t destoryTranHeade(TRAN_HEADE_T *tranHead);

private:
    bool isEmpty(const std::string dirPath);

private:
    ServiceCore() = delete;
    ServiceCore(const ServiceCore &rhs) = delete;
    ServiceCore &operator =(const ServiceCore &rhs) = delete;

private:
    bool                    mConstructed;
    bool                    mSemEnable;
    bool                    mCodesSync;          /* Code synchronization status */
    bool                    mDirctionLoad;
    bool                    mServerConfigStatus;
    ModuleType              mModule;
    ThreadPoolEx            *mThreads;
    SemaphoreTimeout        *mSemTime;
    TRANSFER_STATUS_ENUM    mTranStatus;
    TransferCore            *mTransCore;
    MonitorCore             *mMonitorCore;
    EncryptCore             *mEncryptCore;
    std::string             mLocalPath;
    std::string             mRemotePath;
    // std::map<std::string, uint32_t> &diffFile

};

}

#endif //__SERVICECORE_H__
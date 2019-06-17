#ifndef __SERVICECORE_H__
#define __SERVICECORE_H__

#include <string>
#include <functional>
#include <dirent.h>
#include <sys/types.h>

#include "common.h"
#include "ThreadPoolEx.h"
//#include "transmission/TransferInterface.h"
#include "transmission/TransferCore.h"
#include "monitor/MonitorCore.h"
#include "SemaphoreTimeout.h"

namespace uranium
{

#define EVENT_FLAGE_MASK        (0x4954)
#define DIR_FILE_NAME           ("dirCMD")

typedef enum  TREANFER_EVENT_ENUM {
    CONFIG_EVT = 0xA,
    CONFIG_ACK,
    DIR_MO_EVT,
    DIR_MO_ACK
} TREANFER_EVENT_E;

typedef enum DIR_MO_EVT_ENUM {
    DIR_MO_EVT_LOAD_MD5INFOS,
    DIR_MO_EVT_LOAD,
    DIR_MO_EVT_MODATA
} DIR_MO_EVT_E;

typedef enum DIR_MO_ACK_ENUM {
    DIR_MO_ACK_OK = 0,
    DIR_MO_ACK_EMPTY,
}DIR_MO_ACK_E;

typedef struct TRANSFER_HEADE_TAG {
    uint32_t         flages;
    TREANFER_EVENT_E evtKey;
    uint32_t         evtValue;
} TRAN_HEADE_T;
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
	ServiceCore(TRANSFER_STATUS_ENUM  tranStatus,const std::string localPath, const std::string remotePath);
	virtual~ServiceCore();
    int32_t construct();
    int32_t destruct();

private:
	int32_t transferConfigs(void);
    int32_t transferDictionaryCMD(DIR_MO_EVT_E cmdValue);
	int32_t transferCompleteWorks(void);
	int32_t transferModify(void);
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
    ModuleType              mModule;
    ThreadPoolEx            *mThreads;
    SemaphoreTimeout        *mSemTime;
    TRANSFER_STATUS_ENUM    mTranStatus;
    TransferCore			*mTransCore;
   	std::string 			mLocalPath;
   	std::string 			mRemotePath;
    

};

}

#endif //__SERVICECORE_H__
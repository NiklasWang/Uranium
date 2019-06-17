#include  <iostream>
#include "ServiceCore.h"

namespace uranium
{

int32_t ServiceCore::start()
{
    return mThreads->run(
    []()->int32_t {
        std::cout << "Start runing" << std::endl;
        return NO_ERROR;
    });
}

int32_t ServiceCore::stop()
{
    return mThreads->run(
    []()->int32_t {
        std::cout << "Start runing" << std::endl;
        return NO_ERROR;
    });
}

int32_t ServiceCore::clientInitialize()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        /* step1: --TODO-- send configs file */
        LOGD(mModule," send configs file succeed\n");
        /* --TODO-- */
    }

    /* step2:  check if the local folder is empty*/
    {
        if (SUCCEED(rc)) {
            if (isEmpty(mLocalPath)) {
                /* load source from remote to local */
                rc = transferDictionaryCMD(DIR_MO_EVT_LOAD);
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to load source from remote transferDictionaryCMD\n");
                }
            } else {
                /* ask for remote source codes infors */
                rc = transferDictionaryCMD(DIR_MO_EVT_LOAD_MD5INFOS);
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to load code infors from remote transferDictionaryCMD\n");
                }
            }
        }

        if (SUCCEED(rc)) {
            /* Waiting for message to return */
            // mSemFlage = true;
            /* wait for result returned */
            rc = mSemTime->wait();
            if (FAILED(rc)) {
                LOGE(mModule, "SemaphoreTimeout\n");
            }
        }
    }

    /* step3:  ask for remote is empty */
    mCodesSync = true;
    /* step4:  do nothings */
    return rc;
}

int32_t ServiceCore::serverInitialize()
{
    int32_t rc = NO_ERROR;

    return rc;
}

int32_t ServiceCore::initialize()
{
    int32_t rc = NO_ERROR;

    switch (mTranStatus) {
        case TRAN_CLINET:
            rc = clientInitialize();
            break;
        default:
            rc =  serverInitialize();
            break;
    }
    return rc;
}

int32_t ServiceCore::transferConfigs(void)
{
    int32_t rc = NO_ERROR;
    LOGD(mModule, "transferConfigs is runing\n");
    return rc;
}

TRAN_HEADE_T* ServiceCore::createTranHeade(void)
{
    TRAN_HEADE_T *pTranHead = new TRAN_HEADE_T;
    memset(pTranHead, 0, sizeof(TRAN_HEADE_T));
    pTranHead->flages = EVENT_FLAGE_MASK;
    return pTranHead;
}

int32_t ServiceCore::destoryTranHeade(TRAN_HEADE_T *tranHead)
{
    SECURE_DELETE(tranHead);
    return NO_ERROR;
}

int32_t ServiceCore::transferDictionaryCMD(DIR_MO_EVT_E cmdValue)
{
    int32_t rc = NO_ERROR;
    std::string storagePath = WORK_DIRPATH;

    if (TRAN_CLINET == mTranStatus) {
        rc =  SYS_ERROR;
    }

    if (SUCCEED(rc)) {
        /* --TODO-- Package  the request command */
        TRAN_HEADE_T *pTranHead = createTranHeade();
        if (NOTNULL(pTranHead)) {
            /* Fill load command */
            pTranHead->evtKey = DIR_MO_EVT;
            pTranHead->evtValue = cmdValue;
            storagePath = WORK_DIRPATH;
            storagePath += DIR_FILE_NAME;
            std::ofstream ostream(storagePath, std::ios::binary);
            ostream.write((char *)pTranHead, sizeof(TRAN_HEADE_T));
            ostream.close();
            destoryTranHeade(pTranHead);
            mTransCore->send(storagePath);
        } else {
            LOGE(mModule, "Out of memory\n");
        }
    }

    return rc;
}

int32_t ServiceCore::transferCompleteWorks(void)
{
    int32_t rc = NO_ERROR;

    return rc;
}

int32_t ServiceCore::transferModify(void)
{
    int32_t rc = NO_ERROR;

    return rc;
}

int32_t ServiceCore::lisnenReceiveHandler(std::string &filePath)
{
    int32_t rc = NO_ERROR;
    std::string storagePath = WORK_DIRPATH;
    TRAN_HEADE_T *pTranHead = createTranHeade();

    if(ISNULL(pTranHead)) {
    	rc = NO_MEMORY;
    	LOGE(mModule, "Out of memory\n");
    }

    if(SUCCEED(rc)) {
    	std::ifstream istrm(filePath, std::ios::binary);
    	istrm.read((char*)pTranHead,sizeof(TRAN_HEADE_T));
    }

    destoryTranHeade(pTranHead);

    return rc;
}

int32_t ServiceCore::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        mTransCore = new TransferCore(mTranStatus);
        if (NOTNULL(mTransCore)) {
            rc = mTransCore->construct();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed Core construct mTransCore\n");
            }
            rc = mTransCore->receive(
            	[this](std::string &filePath)-> int32_t{
            		return lisnenReceiveHandler(filePath);
            	});
        }
    }
    if (SUCCEED(rc)) {
        mSemTime = new SemaphoreTimeout(5000);
        if (ISNULL(mSemTime)) {
            rc = SYS_ERROR;
            LOGE(mModule, "Create SemaphoreTimeout failed\n");
        }
    }


    if (SUCCEED(rc)) {
        mConstructed = true;
    }

    return rc;
}

int32_t ServiceCore::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        rc = mTransCore->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed ServiceCore destruct mTransCore\n");
        }
        SECURE_DELETE(mTransCore);
    }

    if (SUCCEED(rc)) {
        mThreads->removeInstance();
    }

    SECURE_DELETE(mSemTime);
    if (SUCCEED(rc)) {
        mSemEnable = false;
        mCodesSync = false;
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

bool ServiceCore::isEmpty(const std::string dirPath)
{
    bool rc = true;
    DIR *dir = opendir(dirPath.c_str());
    struct dirent *ent;
    if (dir == NULL) {
        LOGE(mModule, "seekkey.c-98-opendir\n");
        rc = false;
    }
    if (rc) {
        while (1) {
            ent = readdir(dir);
            if (ent <= 0) {
                break;
            }
            if ((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0)) {
                continue;
            }
            if ((ent->d_type == 4) || (ent->d_type == 8)) {
                rc = false;
                break;
            }
        }
    }

    if (NOTNULL(dir)) {
        closedir(dir);
    }

    return rc;
}

ServiceCore::ServiceCore(TRANSFER_STATUS_ENUM  tranStatus,
                         const std::string localPath, const std::string remotePath):
    mConstructed(false),
    mCodesSync(false),
    //mModule
    mThreads(NULL),
    mTranStatus(tranStatus),
    mLocalPath(localPath),
    mRemotePath(remotePath)
{
    mSemEnable = false;
    mSemTime = NULL;

    std::string cmdStr;
    cmdStr = "mkdir -p ";
    cmdStr += WORK_DIRPATH;
    system(cmdStr.c_str());
}

ServiceCore::~ServiceCore()
{

}

}
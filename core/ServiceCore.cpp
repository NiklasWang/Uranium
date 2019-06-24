#include  <iostream>
#include "ServiceCore.h"
#include "ServiceCore.hpp"

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

    /* step1: --TODO-- send configs file */


#if 0
    /* step2: load dictionary file */
    if (SUCCEED(rc)) {
        mSemEnable = true;
        do {
            std::cout << "\nLHB start runing load transferDictionary\n";
            rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_DIC_LOAD, true);
            rc = mSemTime->wait();
        } while (FAILED(rc)); //(FAILED(rc) || (SUCCEED(mSemTime->wait())));
        mSemEnable = false;
    }
    printf("================== Setp2 load dictionary ok ===================\n");
#endif

    /* step3:  check if the local folder is empty*/
    {
        if (SUCCEED(rc)) {
            if (isEmpty(mLocalPath)) {
                /* load source from remote to local */
                rc = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_LOAD, true);
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to load source from remote transferDictionaryCMD\n");
                }
            } else {
                /* ask for remote source codes infors */
                rc = mMonitorCore->monitorDirInfosScan();
                if (FAILED(rc)) {
                    rc = NOT_INITED;
                    std::cout << "Runing monitorDirInfosScan failed!" << std::endl;
                }

                rc = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_LOAD_MD5INFOS, true);
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed to load code infors from remote transferDictionaryCMD\n");
                }
            }
        }

        if (SUCCEED(rc)) {
            /* Waiting for message to return */
            int32_t tryTimes = 3;
            mSemEnable = true;
            do {
                /* wait for result returned */
                rc = mSemTime->wait();
                if (FAILED(rc)) {
                    std::cout << "SemaphoreTimeout\n";
                }
            } while (FAILED(rc) && (tryTimes--));
            if (tryTimes < 0) {
                std::cout << "Timeout\n";
            }
            mSemEnable = false;
        }
    }
    printf("================== Setp3 code sync succeed ===================\n");

    /* step4:  ask for remote is empty */
    mCodesSync = true;
    /* step4:  do nothings */
    return rc;
}

int32_t ServiceCore::serverInitialize()
{
    int32_t rc = NO_ERROR;
    /* send empty to clean files */
    rc = transferDictionaryCMD(SEND_EMPTY, 0, true);
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

int32_t ServiceCore::transferCompleteWorks(void)
{
    int32_t rc = NO_ERROR;

    return rc;
}

int32_t ServiceCore::transferModify(const std::string& inPath)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        rc = reduceTranHeaderData(inPath, appendBasePath(TRA_SYNC_FILE_NAME));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to prase transfer file\n");
        }
    }

    if (SUCCEED(rc)) {
        rc = praseEntryFile(appendBasePath(TRA_SYNC_FILE_NAME));
    }

    return rc;
}

int32_t ServiceCore::lisnenReceiveHandler(std::string &filePath)
{
    int32_t rc = NO_ERROR;
    // std::string storagePath = WORK_DIRPATH;
    TRAN_HEADE_T *pTranHead = createTranHeade();

    if (ISNULL(pTranHead)) {
        rc = NO_MEMORY;
        printf("Out of memory\n");
    }

    if (SUCCEED(rc)) {
        std::FILE* f = std::fopen(filePath.c_str(), "r");
        if (!f) {
            rc = NO_MEMORY;
            std::cout << "Error fopen file failed\n";
        } else {
            std::fread(pTranHead, 1, sizeof(TRAN_HEADE_T), f);
            std::fclose(f);
        }
    }

    if (SUCCEED(rc)) {

        std::cout << "\nLisnenReceiveHandler flages = " << std::hex << pTranHead->flages << " ";
        std::cout << " Evt_key= 0x" <<  std::hex <<  pTranHead->evtKey;
        std::cout << " Evt_value=0x" << std::hex << pTranHead->evtValue << std::endl;

    }
    if (SUCCEED(rc)) {
        if (pTranHead->flages != EVENT_FLAGE_MASK) {
            rc = NOT_EXIST;
            std::cout << "Transfer data not support\n";
        }
    }

    if (SUCCEED(rc)) {
        switch (pTranHead->evtKey) {
            case CONFIG_EVT:
                /* --TODO-- 处理配置请求返回值 */
                std::cout << "Do CONFIG_EVT handle\n" << std::endl;
                rc = doHandleConEvt(*pTranHead, filePath);
                break;
            case CONFIG_ACK:
                /* --TODO--配置请求返回 */
                std::cout << "Do CONFIG_ACK handle\n" << std::endl;
                rc = doHandleConAck(*pTranHead);
                break;
            case DIR_MO_EVT:
                /* --TODO-- 文件事件请求 */
                std::cout << "Do DIR_MO_EVT handle\n" << std::endl;
                rc  = doHandleMoEvt(*pTranHead, filePath);
                break;
            case DIR_MO_ACK:
                /* --TODO-- 文件事件应答请求 */
                std::cout << "Do DIR_MO_ACK handle\n" << std::endl;
                rc = doHandleMoAck(*pTranHead);
                break;
            default:
                std::cout << "Event not support\n";
                rc = JUMP_DONE;
                break;
        }
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
            [this](std::string & filePath)-> int32_t{
                return lisnenReceiveHandler(filePath);
            });
        }
    }

    if (SUCCEED(rc)) {
        if (mTranStatus == TRAN_CLINET) {
            mMonitorCore = new MonitorCore(mLocalPath);
        } else {
            mMonitorCore = new MonitorCore(mRemotePath);
        }
        if (NOTNULL(mMonitorCore)) {
            rc = mMonitorCore->construct();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed core construct MonitorCore\n");
            }
        }
    }

    {
        if (SUCCEED(rc)) {
            mEncryptCore = new EncryptCore();
            if (NOTNULL(mEncryptCore)) {
                rc =  mEncryptCore->construct();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed core construct mEncryptCore\n");
                }
            }
        }

        if (SUCCEED(rc)) {
            if (mTranStatus != TRAN_CLINET) {
                rc = mEncryptCore->generateDictionKeys();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed runing generateDictionKeys\n");
                } else {
                    mDirctionLoad = true;
                }

            }
        }
    }

    if (SUCCEED(rc)) {
        mSemTime = new SemaphoreTimeout(60000);
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
        mDirctionLoad = false;
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t ServiceCore::tarnsferServer2Clinet(void)
{
    int32_t rc = NO_ERROR;

    std::string  tarFileName(WORK_DIRPATH);

    if (TRAN_CLINET == mTranStatus) {
        tarFileName += CLINET_PATH;
    } else {
        tarFileName += SERVER_PATH;
    }

    if (SUCCEED(rc)) {
        /*  Package all files in the monitoring directory */
        tarFileName += TAR_MODIR_NAME;
        rc = mMonitorCore->monitorTarExec(tarFileName,
        [ = ](void)-> int32_t{
            int32_t __rc = NO_ERROR;
            TRAN_HEADE_T* traHead = createTranHeade();
            if (ISNULL(traHead))
            {
                __rc = NO_MEMORY;
                LOGE(mModule, "Out of memory\n");
            }

            if (SUCCEED(__rc))
            {
                /* Return files to the client */
                traHead->evtKey     = DIR_MO_EVT;
                traHead->evtValue   = DIR_MO_EVT_STORA;
            }

            if (SUCCEED(__rc))
            {
                /* storage file  and send through fex */
                std::string storagePath = WORK_DIRPATH;
                if (TRAN_CLINET == mTranStatus) {
                    storagePath += CLINET_PATH;
                } else {
                    storagePath += SERVER_PATH;
                }
                storagePath += DIR_FILE_NAME;

                std::ofstream ouStream(storagePath, std::ios::binary | std::ios::trunc);
                ouStream.write((char *)traHead, sizeof(TRAN_HEADE_T));
                std::ifstream inStream(tarFileName, std::ios::binary | std::ios::ate);

                auto size = inStream.tellg();
                inStream.seekg(0);
                char *buffer = new char[size];

                if (ISNULL(buffer)) {
                    LOGE(mModule, "Out of memory\n");
                } else {
                    inStream.read(buffer, size);
                    ouStream.write(buffer, size);
                    delete buffer;
                }

                inStream.close();
                ouStream.close();
                destoryTranHeade(traHead);
                mTransCore->send(storagePath);
            }
            return __rc;
        });

        if (FAILED(rc)) {
            LOGE(mModule, "Failed runing mMonitorCore->monitorTarExec filename=%s\n", tarFileName.c_str());
        }
    }
    return rc;
}

int32_t ServiceCore::transferLoadFileInfos()
{
    int32_t rc = NO_ERROR;

    std::string filePath = appendBasePath(FILE_INFOS_NAME);
    std::cout << "file path =" << filePath << std::endl;
    if (SUCCEED(rc)) {
        rc = mMonitorCore->monitorDirInfosScan();
        if (FAILED(rc)) {
            rc = NOT_INITED;
            std::cout << "Runing monitorDirInfosScan failed!" << std::endl;
        }
    }

    if (SUCCEED(rc)) {
        rc = mMonitorCore->monitorDirInfosSave(filePath,
        [ = ]()->int32_t{
            int32_t __rc = NO_ERROR;
            if (SUCCEED(rc))
            {
                __rc  = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_STORA_MD5INFOS, false);
                if (FAILED(__rc)) {
                    std::cout << "Runing transferDictionaryCMD failed\n";
                }
            }
            if (SUCCEED(__rc))
            {
                __rc = transferAppendData(filePath);
                if (FAILED(__rc)) {
                    std::cout << "Runing transferAppendData failed\n";
                }
            }

            if (SUCCEED(rc))
            {
                /* --TODO-- add to core */

            }
            return __rc;
        });
    }

    return rc;
}

int32_t ServiceCore::transferStoraFilelInfos(const std::string &filePath)
{
    int32_t rc = NO_ERROR;
    std::string storagePath = appendBasePath(FILE_INFOS_NAME);
    std::map<std::string, uint32_t> diffFile;

    if (SUCCEED(rc)) {
        rc = reduceTranHeaderData(filePath, storagePath);
        if (FAILED(rc)) {
            std::cout << "Runing reduceTranHeaderData failed\n";
        }
    }

    if (SUCCEED(rc)) {
        rc = mMonitorCore->monitorDirCompareWithLocal(storagePath, diffFile);
        // rc = mMonitorCore->monitorDirInfosLoad(storagePath);
        if (FAILED(rc)) {
            std::cout << "Runing monitorDirInfosLoad failed\n";
        }
    }

    if (SUCCEED(rc)) {
        std::map<std::string, uint32_t>::iterator it;
        if (diffFile.begin() == diffFile.end()) {
            /* do nothing */
        } else {
            rc = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_MODATA, false);
            bool fisrFlage = true;
            for (it = diffFile.begin(); it != diffFile.end(); it++) {
                /* do create files */
                createEntryFile(it->first, it->second, fisrFlage);
                fisrFlage = false;
                // printf("Key=%s value=0x%x\n",it->first.c_str(), it->second);
            }
            rc = transferAppendData(appendBasePath(TRA_SYNC_FILE_NAME));
        }
    }

    return rc;
}

int32_t ServiceCore::praseStora2Local(const std::string &filePath)
{
    return mThreads->run(
    [ = ]()->int32_t {
        std::string storagePath = WORK_DIRPATH;
        if (TRAN_CLINET == mTranStatus)
        {
            storagePath += CLINET_PATH;
        } else
        {
            storagePath += SERVER_PATH;
        }
        storagePath += TAR_MODIR_NAME;

        std::cout << "LHB tar file name = " << storagePath << std::endl;
        std::ofstream ouStream(storagePath, std::ios::binary | std::ios::trunc);
        std::ifstream inStream(filePath, std::ios::binary | std::ios::ate);
        auto size = inStream.tellg();
        if (!ISZERO(size))
        {
            size -= sizeof(TRAN_HEADE_T);
        }
        inStream.seekg(sizeof(TRAN_HEADE_T));
        char *buffer = new char[size];

        if (ISNULL(buffer))
        {
            LOGE(mModule, "Out of memory\n");
        } else
        {
            inStream.read(buffer, size);
            ouStream.write(buffer, size);
            delete buffer;
        }

        inStream.close();
        ouStream.close();
        return mMonitorCore->monitorUntarExec(storagePath);

    });
}

int32_t ServiceCore::doHandleMoEvt(const TRAN_HEADE_T& traHead, const std::string &filePath)
{
    int32_t rc = NO_ERROR;
    switch (traHead.evtValue) {
        case DIR_MO_EVT_LOAD:
            std::cout << "do runing DIR_MO_EVT_LOAD\n";
            /* exam if the dir is empty */
            if (!isEmpty(mRemotePath)) {
                /* 压缩文件并发送返回 */
                rc = transferDictionaryCMD(DIR_MO_ACK, DIR_MO_ACK_OK, true);
                rc = tarnsferServer2Clinet();
            } else {
                /* 发送失败的标志 */
                rc = transferDictionaryCMD(DIR_MO_ACK, DIR_MO_ACK_EMPTY, true);
            }
            break;
        case DIR_MO_EVT_LOAD_MD5INFOS:
            std::cout << "do runing DIR_MO_EVT_LOAD_MD5INFOS\n";
            rc = transferLoadFileInfos();
            break;
        case DIR_MO_EVT_STORA_MD5INFOS:
            std::cout << "do runing DIR_MO_EVT_STORA_MD5INFOS\n";
            rc = transferStoraFilelInfos(filePath);
            break;
        case DIR_MO_EVT_STORA:
            std::cout << "do runing DIR_MO_EVT_STORA\n";
            /* storage file to tmp_files */
            praseStora2Local(filePath);
            break;
        case DIR_MO_EVT_MODATA:
            std::cout << "do runing DIR_MO_EVT_MODATA\n";
            transferModify(filePath);
            break;
        default:
            LOGE(mModule, "DIR_MO_EVT_ENUM not support\n");
            break;
    }

    return rc;
}

int32_t ServiceCore::doHandleMoAck(const TRAN_HEADE_T& traHead)
{
    int32_t rc = NO_ERROR;

    return rc;
}

int32_t ServiceCore::doHandleConEvt(const TRAN_HEADE_T& traHead, const std::string &filePath)
{
    int32_t rc = NO_ERROR;
    std::string  storageFilePath(WORK_DIRPATH);

    if (mTranStatus == TRAN_CLINET) {
        storageFilePath += CLINET_PATH;
    } else {
        storageFilePath += SERVER_PATH;
    }

    std::string tmpStr = storageFilePath;
    switch (traHead.evtValue) {
        case CONFIG_DIC_LOAD:
            std::cout << "do runing DIR_MO_EVT_STORA\n";
            tmpStr = storageFilePath;
            tmpStr += DICTIONARY_NAME;
            rc = mEncryptCore->sotraDiction(tmpStr,
            [ = ]()->int32_t{
                int32_t __rc = NO_ERROR;
                __rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_DIC_STORA, false);
                if (FAILED(rc))
                {
                    LOGE(mModule, "Frailed in transferDictionaryCMD\n");
                }
                if (SUCCEED(rc))
                {
                    __rc = transferAppendData(tmpStr);
                }
                return __rc;
            });
            break;
        case  CONFIG_DIC_STORA: {
            std::cout << "do runing CONFIG_DIC_STORA\n";
            std::string tmpStr = appendBasePath(DICTIONARY_NAME);
            rc = reduceTranHeaderData(filePath, tmpStr);
            /* load dictions */
            mEncryptCore->loadDiction(tmpStr);
            if (mSemEnable) {
                mSemTime->signal();
            }
        }
        break;
        case CONFIG_FILE_TRAN:
            break;

        default:
            LOGE(mModule, "CONFIG_EVT_E not support\n");
            break;
    }
    // DICTIONARY_NAME
    return rc;
}
int32_t ServiceCore::doHandleConAck(const TRAN_HEADE_T& traHead)
{
    int32_t rc = NO_ERROR;

    return rc;
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
    mSemEnable(false),
    mCodesSync(false), // mModule
    mDirctionLoad(false),
    mThreads(NULL),
    mSemTime(NULL),
    mTranStatus(tranStatus),
    mTransCore(NULL),
    mMonitorCore(NULL),
    mEncryptCore(NULL),
    mLocalPath(localPath),
    mRemotePath(remotePath)
{
#if 0
    mSemEnable = false;
    mSemTime = NULL;
    mConstructed = false,      //mModule
    mCodesSync = false,
    mThreads = NULL,
    mMonitorCore = NULL,
    mTransCore = NULL;
#endif
    std::string cmdStr;
    cmdStr = "mkdir -p ";
    cmdStr += WORK_DIRPATH;
    cmdStr += SERVER_PATH;
    system(cmdStr.c_str());
    cmdStr = "mkdir -p ";
    cmdStr += WORK_DIRPATH;
    cmdStr += CLINET_PATH;
    system(cmdStr.c_str());
}

ServiceCore::~ServiceCore()
{

}

}
#include  <iostream>
#include "ServiceCore.h"
#include "ServiceCore.hpp"
#include "Configs.h"
#include <algorithm>

namespace uranium
{

int32_t ServiceCore::start()
{
    return mThreads->run(
    [this]()->int32_t {
        int32_t __rc = NO_ERROR;
        if (TRAN_CLINET == mTranStatus)
        {
            LOGD(mModule, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++");
            if (mStartRuning == true) {
                LOGE(mModule, "Server is already runing");
                return NO_ERROR;
            }

            mStartRuning = true;

            while (mStartRuning) {
                if (mCodesSync) {
                    if (SUCCEED(__rc)) {
                        LOGD(mModule, "ServiceCore Start monitorDirStart");
                        __rc = mMonitorCore->monitorDirStart();
                        if (FAILED(__rc)) {
                            LOGE(mModule, "runing monitorDirStart failed");
                        }
                    }

                    if (SUCCEED(__rc)) {
                        LOGD(mModule, "ServiceCore monitorLoopProcess init");
                        __rc = mMonitorCore->monitorLoopProcess(
                        [this](std::map<std::string, uint32_t>&diffFile)-> int32_t {
                            return monitorDirCallBack(diffFile);
                        });
                        if (FAILED(__rc)) {
                            LOGE(mModule, "monitoringLoopProcess failed");
                        }
                    }
                    break;
                }
                sleep(1);
            }
        }
        LOGD(mModule, "+++++++++++++++++ start end");
        return __rc;
    });
}

int32_t ServiceCore::stop()
{
    return mThreads->run(
    [this]()->int32_t {
        mStartRuning = false;
        sleep(1);
        LOGD(mModule, "Stop runing ....");
        return NO_ERROR;
    });
}

int32_t ServiceCore::clientInitialize()
{
    int32_t rc = NO_ERROR;

    // getUserName();

    /* step1: --TODO-- send configs file */
    /* Waiting for server is ready ok */
    if (SUCCEED(rc)) {
        mSemEnable = true;
        do {
            rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_READY_STATUS, true);
            rc |= mSemTime->wait();
            if (mServerBreak) {
                LOGD(mModule, "mServerBreak is true");
                rc = NOT_FOUND;
            }
        } while (FAILED(rc));
        mSemEnable = false;
    }
    LOGD(mModule, "================== Setp1 load configures ok ===================\n");

    /* step2: load dictionary file */
    if (SUCCEED(rc)) {
        mSemEnable = true;
        do {
            rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_DIC_LOAD, true);
            rc = mSemTime->wait();
            if (mServerBreak) {
                rc = NOT_FOUND;
            }
        } while (FAILED(rc)); //(FAILED(rc) || (SUCCEED(mSemTime->wait())));
        mSemEnable = false;
    }
    LOGD(mModule, "================== Setp2 load dictionary ok ===================\n");

    /* step3:  check if the local folder is empty*/
    {
        if (SUCCEED(rc)) {
            for (auto it = mRemoteDirNames.begin(); it != mRemoteDirNames.end(); it++) {
                std::string dirs = mLocalPath + it->first;
                mActivePath = dirs;
                mActiveName = it->first;
                LOGD(mModule, "Local dirs is %s", dirs.c_str());
                if (isEmpty(dirs)) {
                    LOGD(mModule, "dirs %s is empty need to downloads",  dirs.c_str());
                    /* load source from remote to local */
                    rc = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_LOAD, false);
                    if (FAILED(rc)) {
                        LOGE(mModule, "Failed to load source from remote transferDictionaryCMD");
                    }
                    if (SUCCEED(rc)) {
                        rc = sendReomotePaths(mRemoteDirNames[it->first]);
                        if (FAILED(rc)) {
                            LOGE(mModule, "Runing sendReomotePaths failed");
                        }
                    }
                    if (SUCCEED(rc)) {
                        rc = transferAppendData(appendBasePath(REMOTE_PATH_BIN));
                        if (FAILED(rc)) {
                            LOGE(mModule, "Runing transferAppendData failed");
                        }
                    }
                } else {
                    LOGD(mModule, "dirs %s need to compare md5sums",  dirs.c_str());
                    LOGD(mModule, "start scan dir infos");
                    rc = mMonitorCore->monitorDirInfosScan(dirs);
                    if (FAILED(rc)) {
                        rc = NOT_INITED;
                        LOGE(mModule, "Runing monitorDirInfosScan failed!\n");
                    }
#if 1
                    std::string infosSaveFile = "loadInfos-";
                    infosSaveFile += it->first;
                    infosSaveFile[infosSaveFile.length() - 1] = '.';
                    infosSaveFile += "bin";
                    LOGD(mModule, "========== %s", appendBasePath(infosSaveFile).c_str());
                    mMonitorCore->monitorDirInfosSave(appendBasePath(infosSaveFile), [this]()->int32_t {
                        return 0;
                    });
#endif
                    rc = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_LOAD_MD5INFOS, false);
                    if (FAILED(rc)) {
                        LOGE(mModule, "Failed to load code infors from remote transferDictionaryCMD");
                    }

                    if (SUCCEED(rc)) {
                        rc = sendReomotePaths(mRemoteDirNames[it->first]);
                        if (FAILED(rc)) {
                            LOGE(mModule, "Runing sendReomotePaths failed");
                        }
                    }

                    if (SUCCEED(rc)) {

                        rc = transferAppendData(appendBasePath(REMOTE_PATH_BIN));
                        if (FAILED(rc)) {
                            LOGE(mModule, "Runing transferAppendData failed");
                        }
                    }
                }

                if (SUCCEED(rc)) {
                    /* Waiting for message to return */
                    mSemEnable = true;
                    LOGD(mModule, "Need to enable sem");
                    do {
                        /* wait for result returned */
                        rc = mSemTime->wait();
                        if (FAILED(rc)) {
                            LOGI(mModule, "SemaphoreTimeout");
                        }
                        if (mServerBreak) {
                            LOGD(mModule, "server need to return");
                            rc = NOT_FOUND;
                        }
                    } while (FAILED(rc));
                    mSemEnable = false;
                }
            }
        }

        if (SUCCEED(rc)) {
            mCodesSync = true;
        }

        LOGD(mModule, "================== Setp3 code sync succeed ===================\n");
    }
    LOGD(mModule, "Runing serverInitialize end");
    /* step4:  do nothings */
    return rc;
}

int32_t ServiceCore::serverInitialize()
{
    int32_t rc = NO_ERROR;
    Configs *config = NULL;
    /* send empty to clean files */
    if (SUCCEED(rc)) {
        rc = transferDictionaryCMD(SEND_EMPTY, 0, true);
        if (FAILED(rc)) {
            LOGE(mModule, "Runing transferDictionaryCMD failed\n");
        }
        sleep(1);
    }

    if (SUCCEED(rc)) {
        config = new Configs();
        if (ISNULL(config)) {
            LOGE(mModule, "Create Configs failed(memory out)\n");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        rc = config->load();
        if (SUCCEED(rc)) {
            mServerConfigStatus = true;
        } else {
            mSemEnable = true;
            do {
                rc = mSemTime->wait();
                if (mServerBreak) {
                    rc = NOT_FOUND;
                }
                if (mServerConfigStatus) {
                    rc = config->load();
                    if (SUCCEED(rc)) {
                        mServerConfigStatus = true;
                        break;
                    } else {
                        mServerConfigStatus = false;
                    }
                }
            } while (FAILED(rc));
            mSemEnable = false;
        }
    }

    std::string tmpPath;
    if (SUCCEED(rc)) {

        config->get<std::string>(CONFIG_REMOTE_PATH, tmpPath);
        LOGD(mModule, "remote Path = %s \n", tmpPath.c_str());
    }

    if (SUCCEED(rc)) {
        do {
            auto pos = tmpPath.find(":");
            if (pos == tmpPath.npos) {
                LOGD(mModule, "End path = %s", tmpPath.c_str());
                mRemotePathVector.push_back(tmpPath);
                break;
            }

            auto tmpStr = tmpPath.substr(0, pos);
            mRemotePathVector.push_back(tmpStr);
            LOGI(mModule, " path = %s", tmpStr.c_str());
            tmpPath = tmpPath.substr(pos + 1);
        } while (true);

        /* foreath creat dirs */
        for (auto it = mRemotePathVector.begin(); it != mRemotePathVector.end(); it++) {
            if ((*it)[it->length() - 1] != '/') {
                *it += "/";
            }
            LOGD(mModule, "REMOTE_PATH=%s", it->c_str());
        }

        for (auto it = mRemotePathVector.begin(); it != mRemotePathVector.end(); it++) {
            auto tmpStr = *it;
            if (tmpStr[tmpStr.length() - 1] == '/') {
                tmpStr[tmpStr.length() - 1] = 0;
            }
            auto pos = tmpStr.find_last_of("/");
            if (pos != tmpStr.npos) {
                auto dirNames = it->substr(pos + 1);
                mRemoteDirNames[dirNames] = *it; // .push_back(dirNames);

                LOGE(mModule, "exits dirs %s is empty?", (*it).c_str());
                if (isEmpty(*it)) {
                    LOGE(mModule, "dictionaryary %s is empty or not exits\n", (*it).c_str());
                    rc |= NOT_FOUND;
                }
            }
        }
    }

    if (SUCCEED(rc)) {
        rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_READY_OK, true);
        if (FAILED(rc)) {
            LOGE(mModule, "Runing transferDictionaryCMD failed\n");
        }
    }

    if (SUCCEED(rc)) {
        mMonitorCore = new MonitorCore("./");
        if (NOTNULL(mMonitorCore)) {
            rc = mMonitorCore->construct();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed core construct MonitorCore\n");
            }
        }
    }

    if (SUCCEED(rc)) {
        LOGD(mModule, "================ configures load ok ==================== \n");
    }

    SECURE_DELETE(config);
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
    LOGD(mModule, "FilePath = %s", inPath.c_str());
    if (SUCCEED(rc)) {
        rc = reduceTranHeaderData(inPath, appendBasePath(TRA_SYNC_FILE_NAME));
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to prase transfer file\n");
        }
    }

    if (SUCCEED(rc)) {
        rc = praseEntryFile(appendBasePath(TRA_SYNC_FILE_NAME));
    }

    if (mSemEnable) {
        mSemTime->signal();
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
        LOGE(mModule, "Out of memory\n");
    }

    if (SUCCEED(rc)) {
        std::FILE* f = std::fopen(filePath.c_str(), "r");
        if (!f) {
            rc = NO_MEMORY;
            LOGE(mModule, "Error fopen file failed\n");
        } else {
            std::fread(pTranHead, 1, sizeof(TRAN_HEADE_T), f);
            std::fclose(f);
        }
    }

    if (SUCCEED(rc)) {
        LOGD(mModule, "LisnenReceiveHandler:");
        LOGD(mModule, "flages = 0x%02x  Evt_key= 0x%02x Evt_value=0x%02x",
             pTranHead->flages, pTranHead->evtKey, pTranHead->evtValue);
    }
    if (SUCCEED(rc)) {
        if (pTranHead->flages != EVENT_FLAGE_MASK) {
            rc = NOT_EXIST;
            LOGE(mModule, "Transfer data not support\n");
        }
    }

    if (SUCCEED(rc)) {
        switch (pTranHead->evtKey) {
            case CONFIG_EVT:
                /* --TODO-- 处理配置请求返回值 */
                LOGD(mModule, "Do CONFIG_EVT handle\n");
                rc = doHandleConEvt(*pTranHead, filePath);
                break;
            case CONFIG_ACK:
                /* --TODO--配置请求返回 */
                LOGD(mModule, "Do CONFIG_ACK handle\n");
                rc = doHandleConAck(*pTranHead);
                break;
            case DIR_MO_EVT:
                /* --TODO-- 文件事件请求 */
                LOGD(mModule, "Do DIR_MO_EVT handle\n");
                rc  = doHandleMoEvt(*pTranHead, filePath);
                break;
            case DIR_MO_ACK:
                /* --TODO-- 文件事件应答请求 */
                LOGD(mModule, "Do DIR_MO_ACK handle\n");
                rc = doHandleMoAck(*pTranHead);
                break;
            default:
                LOGE(mModule, "Event not support\n");
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
        LOGE(mModule, "ServiceCore construct has already inited");
        rc = ALREADY_INITED;
    } else {
        mServerBreak = false;
    }

    if (SUCCEED(rc)) {
        mThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mThreads)) {
            LOGE(mModule, "Failed to get thread pool");
            rc = UNKNOWN_ERROR;
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


    if (SUCCEED(rc)) {
        mTransCore = new TransferCore(mTranStatus, mEncryptCore, mName, mPasswd);
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
            if (NOTNULL(mMonitorCore)) {
                rc = mMonitorCore->construct();
                if (FAILED(rc)) {
                    LOGE(mModule, "Failed core construct MonitorCore\n");
                }
            }
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
        LOGE(mModule, "Not inited");
        rc = NOT_INITED;
    } else {
        mConstructed = false;
        mServerBreak = true;
    }

    if (NOTNULL(mMonitorCore)) {
        rc = mMonitorCore->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed core destruct mMonitorCore\n");
        }
        SECURE_DELETE(mMonitorCore);
    }

    if (NOTNULL(mTransCore)) {
        rc = mTransCore->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed ServiceCore destruct mTransCore\n");
        }
        SECURE_DELETE(mTransCore);
    }


    if (NOTNULL(mEncryptCore)) {
        rc = mEncryptCore->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed core destruct mEncryptCore\n");
        }
        SECURE_DELETE(mEncryptCore);

    }

    if (NOTNULL(mThreads)) {
        mThreads->removeInstance();
        mThreads = NULL;
    }


    if (SUCCEED(rc)) {
        mSemEnable = false;
        mCodesSync = false;
        mDirctionLoad = false;
        SECURE_DELETE(mSemTime);
    }

    LOGD(mModule, "MonitorUtils::destruct END ========== ");

    return RETURNIGNORE(rc, NOT_INITED);
}

int32_t ServiceCore::tarnsferServer2Clinet(std::string &filePath)
{
    int32_t rc = NO_ERROR;

    std::string  tarFileName("/tmp/");
    tarFileName += getUserName();

    if (SUCCEED(rc)) {
        /*  Package all files in the monitoring directory */
        /* delete old files */
        std::string cmd;
        cmd = tarFileName;
        rc = folder_mkdirs(cmd.c_str());
        tarFileName += TAR_MODIR_NAME;
        rc = remove(tarFileName.c_str());

        rc = mMonitorCore->monitorTarExec(tarFileName, filePath,
        [ = ](void)-> int32_t{
            int32_t __rc = NO_ERROR;

            if (SUCCEED(__rc))
            {
                __rc = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_STORA, false);
                if (FAILED(__rc)) {
                    LOGE(mModule, "Create transfer header failed\n");
                }
            }

            if (SUCCEED(__rc))
            {
                __rc = transferAppendData(tarFileName);
            }

            // __rc = system(cmd.c_str());
            return __rc;
        });

        if (FAILED(rc)) {
            LOGE(mModule, "Failed runing mMonitorCore->monitorTarExec filename=%s\n", tarFileName.c_str());
        }
    }
    return rc;
}

int32_t ServiceCore::monitorDirCallBack(std::map<std::string, uint32_t>&diffFile)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        std::map<std::string, uint32_t>::iterator it;
        if (diffFile.begin() != diffFile.end()) {
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

int32_t ServiceCore::transferLoadFileInfos(const std::string &dirPath)
{
    int32_t rc = NO_ERROR;

    std::string filePath = appendBasePath(FILE_INFOS_NAME);
    LOGD(mModule, "write file path =%s  dirPath=%s", filePath.c_str(), dirPath.c_str());
    if (SUCCEED(rc)) {
        rc = mMonitorCore->monitorDirInfosScan(dirPath);
        if (FAILED(rc)) {
            LOGE(mModule, "Runing monitorDirInfosScan failed!(rc = %d)", rc);
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
                    LOGE(mModule, "Runing transferDictionaryCMD failed\n");
                }
            }
            if (SUCCEED(__rc))
            {
                __rc = transferAppendData(filePath);
                if (FAILED(__rc)) {
                    LOGE(mModule, "Runing transferAppendData failed\n");
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
            LOGE(mModule, "Runing reduceTranHeaderData failed\n");
        }
    }

    if (SUCCEED(rc)) {
        rc = mMonitorCore->monitorDirCompareWithLocal(storagePath, diffFile);
        // rc = mMonitorCore->monitorDirInfosLoad(storagePath);
        if (FAILED(rc)) {
            LOGE(mModule, "Runing monitorDirInfosLoad failed\n");
        }
    }

    if (SUCCEED(rc)) {
        LOGD(mModule, "Start modify loacal %s files ", mActivePath.c_str());
        std::map<std::string, uint32_t>::iterator it;
        if (diffFile.begin() != diffFile.end()) {
            rc = transferDictionaryCMD(DIR_MO_EVT, DIR_MO_EVT_MODATA, false);
            bool fisrFlage = true;
            for (it = diffFile.begin(); it != diffFile.end(); it++) {
                /* do create files */
                std::string fileRelativePath = mActiveName + it->first;
                LOGD(mModule, "File %s %d ", fileRelativePath.c_str(), it->second);
                createEntryFile(fileRelativePath, it->second, fisrFlage);
                fisrFlage = false;

            }
            LOGD(mModule, "%s %d", __func__, __LINE__);
            rc = transferAppendData(appendBasePath(TRA_SYNC_FILE_NAME));
            sleep(3);
        } else {
            LOGD(mModule, "transferStoraFilelInfos compare ok");
        }
    }

    LOGD(mModule, "Send rc = %d", rc);
    if (SUCCEED(rc)) {
        if (mSemEnable) {
            LOGD(mModule, "Send signal");
            // mCodesSync = true;
            mSemTime->signal();
        }
    }
    return rc;
}

int32_t ServiceCore::praseStora2Local(const std::string &filePath)
{
    return mThreads->run(
    [this, filePath]()->int32_t {
        int32_t __rc = NO_ERROR;
        std::string storagePath = appendBasePath(TAR_MODIR_NAME);

        if (SUCCEED(__rc))
        {
            __rc = reduceTranHeaderData(filePath, storagePath);
            if (FAILED(__rc)) {
                LOGE(mModule, "Runing reduceTranHeaderData failed\n");
            }
        }

        if (SUCCEED(__rc))
        {
            __rc = mMonitorCore->monitorUntarExec(storagePath, mActivePath,
            [this]()->int32_t {
                if (mSemEnable)
                {
                    mCodesSync = true;
                    mSemTime->signal();
                }
                return NO_ERROR;
            });
        }

        return __rc;
    });
}

int32_t ServiceCore::doHandleMoEvt(const TRAN_HEADE_T& traHead, const std::string &filePath)
{
    int32_t rc = NO_ERROR;
    std::string dirPath;
    switch (traHead.evtValue) {
        case DIR_MO_EVT_LOAD:
            LOGD(mModule, "do runing DIR_MO_EVT_LOAD\n");
            /* exam if the dir is empty */
            if (!isEmpty(mLocalPath)) {
                /* 压缩文件并发送返回 */
                rc = transferDictionaryCMD(DIR_MO_ACK, DIR_MO_ACK_OK, true);
                dirPath = praseRemotePath(filePath);
                LOGE(mModule, "origfilePath = %s, dirPath = %s", filePath.c_str(), dirPath.c_str());
                rc = tarnsferServer2Clinet(dirPath);
            } else {
                /* 发送失败的标志 */
                rc = transferDictionaryCMD(DIR_MO_ACK, DIR_MO_ACK_EMPTY, true);
            }
            break;
        case DIR_MO_EVT_LOAD_MD5INFOS:
            LOGD(mModule, "do runing DIR_MO_EVT_LOAD_MD5INFOS\n");
            dirPath = praseRemotePath(filePath);
            LOGE(mModule, "origfilePath = %s, dirPath = %s", filePath.c_str(), dirPath.c_str());
            rc = transferLoadFileInfos(dirPath);
            break;
        case DIR_MO_EVT_STORA_MD5INFOS:
            LOGD(mModule, "do runing DIR_MO_EVT_STORA_MD5INFOS\n");
            rc = transferStoraFilelInfos(filePath);
            break;
        case DIR_MO_EVT_STORA:
            LOGD(mModule, "do runing DIR_MO_EVT_STORA\n");
            /* storage file to tmp_files */
            praseStora2Local(filePath);
            break;
        case DIR_MO_EVT_MODATA:
            LOGD(mModule, "do runing DIR_MO_EVT_MODATA\n");
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
            LOGD(mModule, "do runing DIR_MO_EVT_STORA\n");
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
            LOGD(mModule, "do runing CONFIG_DIC_STORA\n");
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
            LOGD(mModule, "do runing CONFIG_FILE_TRAN\n");
            if (SUCCEED(rc)) {
                rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_FILE_STORA, false);
                if (FAILED(rc)) {
                    LOGE(mModule, "Transfer failed\n");
                }
            }
            if (SUCCEED(rc)) {
                /* --FIXME--  need to change file path*/
                rc = transferAppendData(CONFIG_FILE_NAME);
                if (FAILED(rc)) {
                    LOGE(mModule, "runing transferAppendData failed\n");
                }
            }

            break;

        case CONFIG_FILE_STORA:
            LOGD(mModule, " do runing CONFIG_FILE_STORA\n");
            rc = reduceTranHeaderData(filePath, CONFIG_FILE_NAME);
            mServerConfigStatus = true;
            if (mSemEnable) {
                mSemTime->signal();
            }
            break;
        case CONFIG_READY_STATUS:
            LOGD(mModule, "do runing CONFIG_READY_STATUS\n");
            if (mServerConfigStatus == false) {
                rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_FILE_TRAN, true);
            } else {
                rc = transferDictionaryCMD(CONFIG_EVT, CONFIG_READY_OK, true);
            }
            break;

        case CONFIG_READY_OK:
            LOGD(mModule, "do runing CONFIG_READY_OK\n");
            if (mSemEnable) {
                mSemTime->signal();
            }
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

ServiceCore::ServiceCore(TRANSFER_STATUS_ENUM  tranStatus, const std::string localPath, const std::vector<std::string> remotePaths,
                         const std::string name, const std::string passwd):
    mStartRuning(false),
    mConstructed(false),
    mSemEnable(false),
    mCodesSync(false),
    mDirctionLoad(false),
    mServerConfigStatus(false),
    mModule(MODULE_MONITOR_SERVER),
    mThreads(NULL),
    mSemTime(NULL),
    mTranStatus(tranStatus),
    mTransCore(NULL),
    mMonitorCore(NULL),
    mEncryptCore(NULL),
    mLocalPath(localPath),
    mRemotePathVector(remotePaths),
    mName(name),
    mPasswd(passwd)
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
    int32_t rc = NO_ERROR;
    LOGD(mModule, "MLocalPath origin = %s", mLocalPath.c_str());
    if ('/' != mLocalPath[mLocalPath.size() - 1]) {
        mLocalPath += "/";
    }

#if defined(__CYGWIN__)
    auto point = mLocalPath.find(":");
    if (point != mLocalPath.npos) {
        /* not found point */
        auto tmp_str = mLocalPath.substr(point + 1);
        auto tmp_char = mLocalPath.substr(point - 1, point);
        transform(tmp_char.begin(), tmp_char.end(), tmp_char.begin(), ::tolower);
        mLocalPath = "/cygdrive/" + tmp_char + tmp_str;

        // transform(tmp_char.begin(),tmp_char.end(),tmp_char.begin(),::tolower);
        LOGE(mModule, "MLocalPath dest = %s", mLocalPath.c_str());
        /* cygdrive */
    }
#endif

    folder_mkdirs(mLocalPath.c_str());
    /* foreath creat dirs */
    for (auto it = mRemotePathVector.begin(); it != mRemotePathVector.end(); it++) {
        if ((*it)[it->length() - 1] != '/') {
            *it += "/";
        }
        LOGD(mModule, "REMOTE_PATH=%s", it->c_str());
    }

    for (auto it = mRemotePathVector.begin(); it != mRemotePathVector.end(); it++) {
        auto tmpStr = *it;
        if (tmpStr[tmpStr.length() - 1] == '/') {
            tmpStr[tmpStr.length() - 1] = 0;
        }
        auto pos = tmpStr.find_last_of("/");
        if (pos != tmpStr.npos) {
            auto dirNames = it->substr(pos + 1);
            mRemoteDirNames[dirNames] = *it; // .push_back(dirNames);
            dirNames = mLocalPath + dirNames;
            LOGE(mModule, "Create dirs = %s", dirNames.c_str());
            folder_mkdirs(dirNames.c_str());
        }
    }

    std::string cmdStr;
    cmdStr = WORK_DIRPATH;
    cmdStr += SERVER_PATH;
    rc = folder_mkdirs(cmdStr.c_str());
    if (FAILED(rc)) {
        LOGE(mModule, "Create dictions failed\n");
    }

    cmdStr = WORK_DIRPATH;
    cmdStr += CLINET_PATH;
    rc = folder_mkdirs(cmdStr.c_str());
    if (FAILED(rc)) {
        LOGE(mModule, "Create dictions failed\n");
    }

}

ServiceCore::ServiceCore(TRANSFER_STATUS_ENUM  tranStatus, const std::string localPath, const std::string name, const std::string passwd):
    mStartRuning(false),
    mConstructed(false),
    mSemEnable(false),
    mCodesSync(false),
    mDirctionLoad(false),
    mServerConfigStatus(false),
    mModule(MODULE_MONITOR_SERVER),
    mThreads(NULL),
    mSemTime(NULL),
    mTranStatus(tranStatus),
    mTransCore(NULL),
    mMonitorCore(NULL),
    mEncryptCore(NULL),
    mLocalPath(localPath),
    mName(name),
    mPasswd(passwd)
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
    int32_t rc = NO_ERROR;
    LOGD(mModule, "MLocalPath origin = %s", mLocalPath.c_str());
    if ('/' != mLocalPath[mLocalPath.size() - 1]) {
        mLocalPath += "/";
    }

#if defined(__CYGWIN__)
    auto point = mLocalPath.find(":");
    if (point != mLocalPath.npos) {
        /* not found point */
        auto tmp_str = mLocalPath.substr(point + 1);
        auto tmp_char = mLocalPath.substr(point - 1, point);
        transform(tmp_char.begin(), tmp_char.end(), tmp_char.begin(), ::tolower);
        mLocalPath = "/cygdrive/" + tmp_char + tmp_str;

        // transform(tmp_char.begin(),tmp_char.end(),tmp_char.begin(),::tolower);
        LOGE(mModule, "MLocalPath dest = %s", mLocalPath.c_str());
        /* cygdrive */
    }
#endif

    folder_mkdirs(mLocalPath.c_str());

    std::string cmdStr;
    cmdStr = WORK_DIRPATH;
    cmdStr += SERVER_PATH;
    rc = folder_mkdirs(cmdStr.c_str());
    if (FAILED(rc)) {
        LOGE(mModule, "Create dictions failed\n");
    }

    cmdStr = WORK_DIRPATH;
    cmdStr += CLINET_PATH;
    rc = folder_mkdirs(cmdStr.c_str());
    if (FAILED(rc)) {
        LOGE(mModule, "Create dictions failed\n");
    }

}

ServiceCore::~ServiceCore()
{
    std::string workPath = WORK_DIRPATH;
    workPath += SERVER_PATH;
    /* --TODO-- 循环删除文件 */
#if 0
    LOGD(mModule, "Path = %s", workPath.c_str());
    remove(workPath.c_str());
    workPath = WORK_DIRPATH;
    workPath +=  CLINET_PATH;
    remove(workPath.c_str());

    workPath = WORK_DIRPATH;
    remove(workPath.c_str());
#endif
}

}

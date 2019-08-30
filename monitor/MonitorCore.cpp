#include "MonitorCore.h"

namespace uranium
{

bool MonitorCore::monitorDirNotExit(void)
{
    return mThreads->run(
    [this]() -> bool {
        return mFileMage->dirNotExit();});
}

bool MonitorCore::monitorDirCompareWithLocal(const std::string file, \
        std::map<std::string, uint32_t> &diffFile)
{
    return mFileMage->dirCompareWithLocal(file, diffFile);
}

int32_t MonitorCore::monitorTarExec(const std::string &files, const std::string &dirPath, std::function<int32_t ()>cb)
{
    return mThreads->run(
    [this, files, dirPath, cb]() -> int32_t{
        mFileMage->fileTarFromPath(files, dirPath);
        return cb();
    });
}

int32_t MonitorCore::monitorUntarExec(const std::string &files, const std::string &dirPath, std::function<int32_t ()>cb)
{
    return mThreads->run(
    [this, files, dirPath, cb]() -> int32_t{
        mFileMage->fileUntarToPath(files, dirPath);
        return cb();
    });
}

int32_t MonitorCore::monitorDirInfosSave(const std::string path, std::function<int32_t (void)> cb)
{
    return mThreads->run(
    [this, path, cb]() -> int32_t{
        mFileMage->fileInfosSave(path);
        return cb();
    });
}

int32_t MonitorCore::monitorDirInfosLoad(const std::string path)
{
    return mThreads->run(
    [this, path]() -> int32_t{
        return mFileMage->fileInfosLoad(path);
    });
}

int32_t MonitorCore::monitorDirInfosScan(void)
{
    return  mFileMage->fileScanToInis();
}

int32_t MonitorCore::monitorDirStart(void)
{
    if (mMoniStarFlag) {
        LOGE(mModule, "monitorDirStart has already runing....");
        return mMoniStarFlag;
    }
    mMoniStarFlag = true;
    return mThreads->run(
    [this]() -> int32_t{
        LOGD(mModule, "monitor start dir runing....");
        return mMonitor->start();
    });
}

int32_t MonitorCore::monitorDirStop(void)
{
    if (!mMoniStarFlag) {
        LOGE(mModule, "monitorDirStop had already runing...");
        return mMoniStarFlag;
    }
    mMoniStarFlag = false;
    return mThreads->run(
    [this]() -> int32_t {
        LOGD(mModule, "monitor stop dir runing....");
        return mMonitor->stop();
    });
}

int32_t MonitorCore::monitorLoopProcess(std::function<int32_t (std::map<std::string, uint32_t>&) > cb)
{


    if (mLoopRuning) {
        LOGE(mModule, "function has already runing....\n");
        return -1;
    } else {
        mLoopRuning = true;
    }

    return mThreads->run(
    [this, cb]()-> int32_t {
        std::map<std::string, uint32_t> tmpDiffFile;
        std::map<std::string, MONITOR_FILES_T>::iterator iter;
        std::map<std::string, uint32_t>::iterator iter_tmp;
        do
        {
            sleep(3);
            pthread_mutex_lock(&infoMutex);

            for (iter = mFileModify.begin(); iter != mFileModify.end();) {
                if (iter->second.envFlages & (MONITOR_Created | MONITOR_Updated | MONITOR_MovedTo)) {
                    tmpDiffFile[iter->first] = MONITOR_Updated;
                } else if (iter->second.envFlages & (MONITOR_Removed | MONITOR_Renamed)) {
                    tmpDiffFile[iter->first] = MONITOR_Removed;
                }
                LOGI(mModule, "%s %d", iter->first.c_str(), iter->second.envFlages);
                iter = mFileModify.erase(iter);
            }
            // mFileModify[key] = value;
            pthread_mutex_unlock(&infoMutex);

            cb(tmpDiffFile);
            /* clear tmpDirfile */
            for (iter_tmp = tmpDiffFile.begin(); iter_tmp != tmpDiffFile.end();) {
                iter_tmp = tmpDiffFile.erase(iter_tmp);
            }

        } while (mLoopRuning);
        return NO_ERROR;
    });
}

int32_t MonitorCore::monitorLoopStop(void)
{
    mLoopRuning = false;
    sleep(3);
    return NO_ERROR;
}

int32_t MonitorCore::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        LOGE(mModule, "MonitorCore has already inited");
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
        mFileMage = new FileManager(mMonitorPath);
        if (ISNULL(mFileMage)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Failed create core fileManager\n");
        } else {
            rc = mFileMage->construct();
            if (FAILED(rc)) {
                LOGE(mModule, "Filed to construct core FileManager\n");
            }
        }

    }

    if (SUCCEED(rc)) {
        std::vector<std::string> path;
        path.push_back(mMonitorPath);
        mMonitor = new MonitorUtils(path, [this](const std::vector<event>& envets)->void {
            processHandle(envets);
        });
        if (NOTNULL(mMonitor)) {
            rc = mMonitor->construct();
            if (FAILED(rc)) {
                LOGE(mModule, "Failed to construct core MonitorUtils\n");
            }
        } else {
            LOGE(mModule, "Failed to create core MonitorUtils\n");
        }
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }
    return rc;
}

int32_t MonitorCore::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        LOGE(mModule, "MonitorCore destruct failed with not inited");
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (mLoopRuning) {
        monitorLoopStop();
    }

    if (NOTNULL(mMonitor)) {
        rc = mMonitor->stop();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to stop core mMonitor\n");
        }

        rc = mMonitor->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct core mMonitor\n");
        }
        SECURE_DELETE(mMonitor);
    }

    if (NOTNULL(mFileMage)) {
        rc = mFileMage->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct core mFileMage\n");
        }

        SECURE_DELETE(mFileMage);
    }


    {
        mThreads->removeInstance();
        mThreads = NULL;
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

void MonitorCore::processHandle(const std::vector<event>& events)
{
    /* --TODO--  */
    int32_t rc = 0;
    uint32_t envFlages = 0;
    for (const event& evt : events) {

        rc = mMonitor->filtrationEvents(evt, envFlages);
        if (FAILED(rc)) {
            continue;
        }
        rc = mMonitor->filtrationEvents(evt.get_path());
        if (FAILED(rc)) {
            continue;
        }

        std::string key = evt.get_path();

        /*
                     --FIXME--
            do not differentiate dir or files
            need to do somethings different
        */
        MONITOR_FILES_T value;
        value.envFlages = envFlages;
        value.time =    evt.get_time();

        auto point = key.find(mMonitorPath);
        if (point == key.npos) {
            LOGE(mModule, "DANGER***** not find[ %s] in [ %s]", mMonitorPath.c_str(), key.c_str());
            continue;
        }
        point += mMonitorPath.length();
        auto tmp_str = key.substr(point);

        if (tmp_str.at(0) == '/') {
            tmp_str = tmp_str.substr(1);
        }
        key = tmp_str;

        auto result = mFileModify.find(key);
        /* correctly find key-value */
        if (result != mFileModify.end()) {
            /* updata eventFlages */
            value.envFlages |= result->second.envFlages;
        }

        pthread_mutex_lock(&infoMutex);
        mFileModify[key] = value;
        pthread_mutex_unlock(&infoMutex);
#if 0
        /* DEBUG used */
        std::cout << "begin:-------------------" << std::endl;
        std::map<std::string, MONITOR_FILES_T>::iterator iter;
        for (iter = mFileModify.begin(); iter != mFileModify.end(); iter++) {
            std::cout << iter->first << "=" << std::hex << iter->second.envFlages << std::endl;
            // ostream << iter->first << "=" << mFileInfos[iter->first] << std::endl;
        }
        std::cout << " endl:-------------------" << std::endl;
#endif
    }
}

MonitorCore::MonitorCore(std::string monitorPath):
    mConstructed(false),
    mLoopRuning(false),
    mMoniStarFlag(false),
    mModule(MODULE_MONITOR),
    // mFileMagePath(fileMagePath),
    mMonitorPath(monitorPath),
    mThreads(NULL)
{
    pthread_mutex_init(&infoMutex, NULL);
}

MonitorCore::~MonitorCore()
{
    SECURE_DELETE(mMonitor);
    SECURE_DELETE(mFileMage);
    if (NOTNULL(mThreads)) {
        mThreads->removeInstance();
        mThreads = NULL;
    }
    pthread_mutex_destroy(&infoMutex);
}

}

/**
 * @file MonitorCore.cpp
 * @brief
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-06-07
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */
#include "MonitorCore.h"


namespace uranium
{

bool MonitorCore::monitorDirNotExit(void)
{
    return mThreads->run(
    [this]() -> bool {
        return mFileMage->dirNotExit();});
}

bool MonitorCore::monitorDirCompareWithLocal(const std::string file)
{
    return mThreads->run(
    [this, file]() -> bool{
        return mFileMage->dirCompareWithLocal(file);});
}

int32_t MonitorCore::monitorTarExec(const std::string files)
{
    return mThreads->run(
    [this, files]() -> int32_t{
        return mFileMage->fileTarFromPath(files);});
}

int32_t MonitorCore::monitorUntarExec(const std::string files)
{
    return mThreads->run(
    [this, files]() -> int32_t{
        return mFileMage->fileUntarToPath(files);
    });
}

int32_t MonitorCore::monitorDirInfosSave(const std::string path)
{
    return mThreads->run(
    [this, path]() -> int32_t{
        return mFileMage->fileInfosSave(path);
    });
}

int32_t MonitorCore::monitorDirInfosLoad(const std::string path)
{
    return mThreads->run(
    [this, path]() -> int32_t{
        return mFileMage->fileInfosLoad(path);
    });
}

int32_t MonitorCore::monitorDirStart(void)
{
    if (mMoniStarFlag) {
        return mMoniStarFlag;
    }
    std::cout << std::endl << "start runing" << std::endl;

    return mThreads->run(
    [this]() -> int32_t{
        return mMonitor->start();
    });
}

int32_t MonitorCore::monitorDirStop(void)
{
    if (!mMoniStarFlag) {
        return mMoniStarFlag;
    }
    return mThreads->run(
    [this]() -> int32_t {
        return mMonitor->stop();
    });
}

int32_t MonitorCore::monitorLoopProcess(void)
{

    if (mLoopRuning) {
        LOGE(mModule, "function has already runing....\n");
        return -1;
    } else {
        mLoopRuning = true;
    }

    return mThreads->run(
    [this]()-> int32_t {
        do
        {
            sleep(0.1);
        } while (mLoopRuning);
        return NO_ERROR;
    });
}

int32_t MonitorCore::monitorLoopStop(void)
{
    mLoopRuning = false;
    sleep(2);
    return NO_ERROR;
}

int32_t MonitorCore::construct()
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
        mFileMage = new FileManager(mFileMagePath);
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
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (mLoopRuning) {
        monitorLoopStop();
    }

    {
        rc = mMonitor->stop();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to stop core mMonitor\n");
        }

        rc = mMonitor->destruct();
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to destruct core mMonitor\n");
        } else {
            SECURE_DELETE(mMonitor);
        }
    }

    rc = mFileMage->destruct();
    if (FAILED(rc)) {
        LOGE(mModule, "Failed to destruct core mFileMage\n");
    } else {
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
        /*             --FIXME--
            do not differentiate dir or files
            need to do somethings different
        */
        MONITOR_FILES_T value;
        value.envFlages = envFlages;
        value.time =    evt.get_time();
        auto result = mFileModify.find(key);
        /* correctly find key-value */
        if (result != mFileModify.end()) {
            /* updata eventFlages */
            value.envFlages |= result->second.envFlages;
        }
        pthread_mutex_lock(&infoMutex);
        mFileModify[key] = value;
        pthread_mutex_unlock(&infoMutex);

        /* DEBUG used */
        std::cout << "begin:-------------------" << std::endl;
        std::map<std::string, MONITOR_FILES_T>::iterator iter;
        for (iter = mFileModify.begin(); iter != mFileModify.end(); iter++) {
            std::cout << iter->first << "=" << std::hex << iter->second.envFlages << std::endl;
            // ostream << iter->first << "=" << mFileInfos[iter->first] << std::endl;
        }
        std::cout << " endl:-------------------" << std::endl;
    }
}

MonitorCore::MonitorCore(std::string fileMagePath, std::string monitorPath):
    mConstructed(false),
    mLoopRuning(false),
    mMoniStarFlag(false),
    mFileMagePath(fileMagePath),
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
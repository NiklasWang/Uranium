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
        return mFileMage->dirNotExit();
    }
           );
}

bool MonitorCore::monitorDirCompareWithLocal(const std::string file)
{
    return mThreads->run(
    [this, file]() -> bool{
        return mFileMage->dirCompareWithLocal(file);
    }
           );
}

int32_t MonitorCore::monitorTarExec(const std::string files)
{
    return mThreads->run(
    [this, files]() -> int32_t{
        return mFileMage->fileTarFromPath(files);
    }
           );
}

int32_t MonitorCore::monitorUntarExec(const std::string files)
{
    return mThreads->run(
    [this, files]() -> int32_t{
        return mFileMage->fileUntarToPath(files);
    }
           );
}

int32_t MonitorCore::monitorDirInfosSave(const std::string path)
{
    return mThreads->run(
    [this, path]() -> int32_t{
        return mFileMage->fileInfosSave(path);
    }
           );
}
int32_t MonitorCore::monitorDirInfosLoad(const std::string path)
{
    return mThreads->run(
    [this, path]() -> int32_t{
        return mFileMage->fileInfosLoad(path);
    }
           );
}

int32_t MonitorCore::monitorLoopProcess(void)
{
    int32_t rc = NO_ERROR;
    while (true) {

    }
    return rc;
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
        mLoopThreads = ThreadPoolEx::getInstance();
        if (ISNULL(mLoopThreads)) {
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
        mMonitor = new MonitorUtils(path,
                                    [this](const std::vector<event>& envets)->void {processHandle(envets);});
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

    rc = mMonitor->destruct();
    if (FAILED(rc)) {
        LOGE(mModule, "Failed to destruct core mMonitor\n");
    } else {
        SECURE_DELETE(mMonitor);
    }

    rc = mFileMage->destruct();
    if (FAILED(rc)) {
        LOGE(mModule, "Failed to destruct core mFileMage\n");
    } else {
        SECURE_DELETE(mFileMage);
    }

    {
        mLoopThreads->removeInstance();
        mLoopThreads = NULL;
        mThreads->removeInstance();
        mThreads = NULL;
    }

    return RETURNIGNORE(rc, NOT_INITED);
}

void MonitorCore::processHandle(const std::vector<event>& envets)
{
    /* --TODO--  */
}

MonitorCore::MonitorCore(std::string fileMagePath, std::string monitorPath):
    mConstructed(false),
    mFileMagePath(fileMagePath),
    mMonitorPath(monitorPath),
    mThreads(NULL),
    mLoopThreads(NULL)
{

}

MonitorCore::~MonitorCore()
{
    SECURE_DELETE(mMonitor);
    SECURE_DELETE(mFileMage);
    if (NOTNULL(mThreads)) {
        mThreads->removeInstance();
        mThreads = NULL;
    }
    if (NOTNULL(mLoopThreads)) {
        mLoopThreads->removeInstance();
        mLoopThreads = NULL;
    }
}

}
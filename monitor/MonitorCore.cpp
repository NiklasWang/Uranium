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
        mMonitor = new MonitorUtils(path,
                                    [this](const std::vector<event>& envets)->void {processHandle(envets);});
        if (NOTNULL(mMonitor)) {
            /// rc = mMonitor->construct();
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

    return rc;
}

void MonitorCore::processHandle(const std::vector<event>& envets)
{
    /* --TODO--  */
}

MonitorCore::MonitorCore(std::string fileMagePath, std::string monitorPath):
    mConstructed(false),
    mFileMagePath(fileMagePath),
    mMonitorPath(monitorPath)
{

}
MonitorCore::~MonitorCore()
{

}

}
/**
 * @file MonitorUitl.cpp
 * @brief
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-06-03
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */
#include "common.h"
#include "MonitorUtils.h"

namespace uranium
{

// static FSW_EVENT_CALLBACK processEvnets;
void processEvnets(const std::vector<event>& events, void *context)
{
    int32_t rc = 0;
    uint32_t envFlages = 0;
    MonitorUtils *pMointor = (MonitorUtils*) context;

    for (const event& evt : events) {
        rc = pMointor->filtrationEvents(evt, envFlages);
        if (FAILED(rc)) {
            continue;
        }

        time_t evtTime = evt.get_time();
        std::string path = evt.get_path();

        MONITOR_FILES_T tmpMoFile;
        tmpMoFile.time = evtTime;
        tmpMoFile.filePath = path;
        tmpMoFile.envFlages = envFlages;
        /* --TODO--  add mutex to protect this contex */
        pMointor->mMonitorFiles.push_back(tmpMoFile);
    }

}

MonitorUtils::MonitorUtils()
{

}

MonitorUtils::~MonitorUtils()
{

}

int32_t MonitorUtils::getMonitorFile(std::vector<MONITOR_FILES_T> &monitorFile)
{
    int32_t rc = 0;
    MONITOR_FILES_T tmpMoFile;

    if (ISZERO(mMonitorFiles.size())) {
        return rc;
    }

    if (SUCCEED(rc)) {
        do {
            /* Filter the same suspension */
            std::vector<MONITOR_FILES_T>::iterator iv;
            iv = mMonitorFiles.begin();
            tmpMoFile = *iv;
            mMonitorFiles.erase(iv);
            for (iv = mMonitorFiles.begin(); iv != mMonitorFiles.end();) {
                if ((iv->filePath == tmpMoFile.filePath) && (iv->envFlages == tmpMoFile.envFlages)) {
                    if (iv->time >= tmpMoFile.time) {
                        tmpMoFile = *iv;
                    }
                    iv = mMonitorFiles.erase(iv);
                } else {
                    ++iv;
                }
            }
            /* storage item to monitorFile */
            monitorFile.push_back(tmpMoFile);
        } while (!ISZERO(mMonitorFiles.size()));
    }


    if (SUCCEED(rc)) {

    }
    return rc;

}

void MonitorUtils::setMonitorDirPath(const std::vector<std::string> path)
{

    for (uint32_t i = 0; i < path.size(); i++) {
        std::string tmpStr(fsw_realpath(path[i].c_str(), nullptr));
        mPaths.push_back(tmpStr);
    }

}

void MonitorUtils::closeMonitor()
{
    if (!ISNULL(mActiveMonitor)) {
        mActiveMonitor->stop();
    }
}

void MonitorUtils::startMonitor(const std::vector<std::string> path)
{
    int32_t rc = 0;

    /* storage path */
    if (SUCCEED(rc)) {

        for (uint32_t i = 0; i < path.size(); i++) {
            std::string tmpStr(fsw_realpath(path[i].c_str(), nullptr));
            mPaths.push_back(tmpStr);
        }

    }

    if (SUCCEED(rc)) {
        mActiveMonitor = monitor_factory::create_monitor(
                             fsw_monitor_type::system_default_monitor_type,
                             mPaths,
                             processEvnets,
                             this);

        if (ISNULL(mActiveMonitor)) {
            rc = -1;
            LOGE(mModule, "create monitor_factory failed!\n");
        }
    }

    if (SUCCEED(rc)) {
        mActiveMonitor->set_recursive(true);
    }

    /* if succeed this funcion not return */
    if (SUCCEED(rc)) {
        mActiveMonitor->start();
        rc = -1;
    }

    if (FAILED(rc)) {
        delete mActiveMonitor;
        mActiveMonitor = NULL;
    }
}


int32_t MonitorUtils::filtrationEvents(event event, uint32_t &evnFlage)
{
    int rc = 0;
    uint32_t tmpEnvFlag = 0;
    const uint32_t filtrationMask = (MONITOR_Created + MONITOR_Updated +
                                     MONITOR_Removed + MONITOR_Renamed  + MONITOR_MovedFrom + MONITOR_MovedTo);

    const std::vector<fsw_event_flag>& flags = event.get_flags();
    for (const fsw_event_flag& flag : flags) {
        tmpEnvFlag += static_cast<uint32_t>(flag);
    }

    if (tmpEnvFlag & filtrationMask) {
        evnFlage = tmpEnvFlag;
    } else {
        rc = -1;
    }

    return rc;
}

};  /* class MonitorUtils */
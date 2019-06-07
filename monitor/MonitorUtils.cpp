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
#if 0
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
#endif

int32_t MonitorUtils::stop()
{

    if (!ISNULL(mActiveMonitor)) {
        mActiveMonitor->stop();
    }

    return NO_ERROR;
}

int32_t MonitorUtils::start()
{
    int32_t rc = NO_ERROR;

    /* if succeed this funcion not return */
    if (SUCCEED(rc)) {
        mActiveMonitor->start();
        rc = -SYS_ERROR;
    }

    return rc;
}

int32_t MonitorUtils::construct(FSW_EVENT_CALLBACK *cb)
{
    int32_t rc = NO_ERROR;
    if (mConstructed) {
        rc = ALREADY_INITED;
    } else {
        if (ISNULL(mActiveMonitor)) {
            mActiveMonitor = monitor_factory::create_monitor(
                                 fsw_monitor_type::system_default_monitor_type,
                                 mPaths,
                                 *cb,
                                 nullptr);
            if (ISNULL(mActiveMonitor)) {
                rc = SYS_ERROR;
                LOGE(mModule, "create monitor_factory failed!\n");
            }
        }
    }

    if (SUCCEED(rc)) {
        mActiveMonitor->set_recursive(true);
    }

    return rc;
}

int32_t MonitorUtils::destruct()
{
    int32_t rc  = NO_ERROR;
    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    return rc;
}

MonitorUtils::MonitorUtils(const std::vector<std::string> path):
    mConstructed(false),
    mPaths(path),
    mActiveMonitor(nullptr)
    // mModule(1)
{

}

MonitorUtils::~MonitorUtils()
{
    SECURE_DELETE(mActiveMonitor);
}

}  /* class MonitorUtils */
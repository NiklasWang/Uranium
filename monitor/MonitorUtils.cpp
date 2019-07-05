#include "common.h"
#include "MonitorUtils.h"

namespace uranium
{

// static FSW_EVENT_CALLBACK processEvnets;
//void MonitorUtils::porcessEnvet(const std::function<void (std::vector<event>&, void*) > cb)
void MonitorUtils::porcessEnvet(const std::vector<event>& events, void *context)
{
    MonitorUtils *tmpPrt = (MonitorUtils*) context;
    tmpPrt->mFunc(events);
}

int32_t MonitorUtils::filtrationEvents(const std::string filePath)
{
    int32_t rc = NO_ERROR;
    std::string::size_type point;
    std::string  tmpStr;

    if (SUCCEED(rc)) {
        point = filePath.find('.');
        if (point != std::string::npos) {
            std::string tmpStr = filePath.substr(0, point);
            if (tmpStr.empty()) {
                rc = -1;
            } else if (tmpStr.at(point - 1) == '/') {
                rc = -1;
            }
        }
    }

    if (SUCCEED(rc)) {

    }

    return rc;
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

int32_t MonitorUtils::stop()
{
    mActiveMonitor->stop();
    sleep(0.5);
    return NO_ERROR;
}

int32_t MonitorUtils::start()
{
    int32_t rc = NO_ERROR;

    /* if succeed this funcion not return */
    if (SUCCEED(rc)) {
        mActiveMonitor->start();
    }

    return rc;
}

int32_t MonitorUtils::construct()
{
    int32_t rc = NO_ERROR;
    if (mConstructed) {
        rc = ALREADY_INITED;
    } else {
        if (ISNULL(mActiveMonitor)) {
            mActiveMonitor = monitor_factory::create_monitor(
                                 fsw_monitor_type::system_default_monitor_type,
                                 mPaths,
                                 porcessEnvet,
                                 this);
            if (ISNULL(mActiveMonitor)) {
                rc = SYS_ERROR;
                LOGE(mModule, "create monitor_factory failed!\n");
            }
        }
    }

    if (SUCCEED(rc)) {
        LOGD(mModule, "Set monitors param\n");
        mActiveMonitor->set_allow_overflow(false);
        mActiveMonitor->set_latency(true);
        // mActiveMonitor->set_recursive(ture);
        mActiveMonitor->set_fire_idle_event(false);
        mActiveMonitor->set_recursive(false);
        mActiveMonitor->set_directory_only(false);
        // mActiveMonitor->set_event_type_filters(event_filters);
        // mActiveMonitor->set_filters(filters);
        mActiveMonitor->set_follow_symlinks(false);
        mActiveMonitor->set_watch_access(false);
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

MonitorUtils::MonitorUtils(const std::vector<std::string> path, std::function<void (const std::vector<event>&)> func):
    mConstructed(false),
    mPaths(path),
    mActiveMonitor(nullptr),
    mModule(MODULE_MONITOR),
    mFunc(func)
    // mModule(1)
{

}

MonitorUtils::~MonitorUtils()
{
    SECURE_DELETE(mActiveMonitor);
}

}  /* class MonitorUtils */
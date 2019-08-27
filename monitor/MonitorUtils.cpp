#include "common.h"
#include "MonitorUtils.h"

namespace uranium
{

void porcessEnvetC(fsw_cevent const *const events,
                   const unsigned int event_num,
                   void *data)
{
    MonitorUtils *tmpPrt = (MonitorUtils*) data;
    std::string path(events->path);
    std::vector<fsw_event_flag> flags;
    flags.push_back(*(events->flags));
    event  eventClinet(path, events->evt_time, flags);
    std::vector<event> eventVect;
    eventVect.push_back(eventClinet);

    if (tmpPrt->mFunc) {
        tmpPrt->mFunc(eventVect);
    }

}
// static FSW_EVENT_CALLBACK processEvnets;
//void MonitorUtils::porcessEnvet(const std::function<void (std::vector<event>&, void*) > cb)
void MonitorUtils::porcessEnvet(const std::vector<event>& events, void *context)
{
    MonitorUtils *tmpPrt = (MonitorUtils*) context;
    if (tmpPrt->mFunc) {
        tmpPrt->mFunc(events);
    }
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
    LOGD(mModule, "MonitorUtils stop *************************");
#if 0
    mActiveMonitor->stop();
    sleep(0.5);
#else
    fsw_stop_monitor(handle);
#endif
    return NO_ERROR;
}

int32_t MonitorUtils::start()
{
    int32_t rc = NO_ERROR;

    /* if succeed this funcion not return */
    if (SUCCEED(rc)) {
        LOGD(mModule, "MonitorUtils start *************************");
#if 0
        mActiveMonitor->start();
#else
        fsw_start_monitor(handle);
#endif
    }

    return rc;
}

int32_t MonitorUtils::construct()
{
    int32_t rc = NO_ERROR;
    if (mConstructed) {
        LOGE(mModule, "construct has ALREADY_INITED");
        rc = ALREADY_INITED;
    } else {
        if (ISNULL(mActiveMonitor)) {
#if 0
            mActiveMonitor = monitor_factory::create_monitor(
                                 fsw_monitor_type::system_default_monitor_type,
                                 mPaths,
                                 porcessEnvet,
                                 this);
            if (ISNULL(mActiveMonitor)) {
                rc = SYS_ERROR;
                LOGE(mModule, "create monitor_factory failed!\n");
            }
#else
            fsw_init_library();
            handle = fsw_init_session(system_default_monitor_type);
            fsw_add_path(handle, mPaths[0].c_str());
            fsw_set_callback(handle, porcessEnvetC, this);
#endif
        }
    }

    if (SUCCEED(rc)) {
        LOGD(mModule, "mActiveMonitor set monitors param\n");
#if 0
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
#else
        fsw_set_allow_overflow(handle, false);
        fsw_set_latency(handle, false);
        fsw_set_directory_only(handle, false);
        fsw_set_follow_symlinks(handle, false);
#endif
    }

    if (SUCCEED(rc)) {
        mConstructed = true;
    }
    return rc;
}

int32_t MonitorUtils::destruct()
{
    int32_t rc  = NO_ERROR;
    if (!mConstructed) {
        LOGE(mModule, "MonitorUtils is not inited");
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (NOTNULL(mFunc)) {
        mFunc = nullptr;
    }

#if 0
    if (NOTNULL(mActiveMonitor)) {
        // mActiveMonitor->stop();
        /** --FIXME-- 无法删除 **/
        try {
            while (mActiveMonitor->is_running()) {
                LOGE(mModule, "mActiveMonitor is_running() wait for stop");
                mActiveMonitor->stop();
                sleep(1);
            }
            LOGE(mModule, "Stoped mActiveMonitors ");
            delete mActiveMonitor;
            mActiveMonitor = nullptr;
            // SECURE_DELETE(mActiveMonitor);
        } catch (libfsw_exception& lex) {
            LOGE(mModule, "Failed ...... ");
            LOGE(mModule, "%s Status code: 0x%x", lex.what(), lex.error_code());
        }
    }
#endif
    fsw_stop_monitor(handle);
    fsw_destroy_session(handle);

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

}

}  /* class MonitorUtils */
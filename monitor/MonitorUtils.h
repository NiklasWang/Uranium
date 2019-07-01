#ifndef __MONITORUTILS_H__
#define __MONITORUTILS_H__
#include "libfswatch/gettext.h"
#include "libfswatch/fswatch.hpp"
#include <iostream>
#include <string>
#include <exception>
#include <csignal>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <cerrno>
#include <vector>
#include <map>
#include <stdint.h>
#include <functional>
#include "libfswatch/c++/path_utils.hpp"
#include "libfswatch/c++/event.hpp"
#include "libfswatch/c++/monitor.hpp"
#include "libfswatch/c++/monitor_factory.hpp"
#include "libfswatch/c/error.h"
#include "libfswatch/c/libfswatch.h"
#include "libfswatch/c/libfswatch_log.h"
#include "libfswatch/c++/libfswatch_exception.hpp"
#include "common.h"

namespace uranium
{
using namespace fsw;
typedef enum  MONITOR_EVE_ENUM {
    MONITOR_NoOp = 0,                     /**< No event has occurred. */
    MONITOR_PlatformSpecific = (1 << 0),  /**< Platform-specific placeholder for event type that cannot currently be mapped. */
    MONITOR_Created = (1 << 1),           /**< An object was created. */
    MONITOR_Updated = (1 << 2),           /**< An object was updated. */
    MONITOR_Removed = (1 << 3),           /**< An object was removed. */
    MONITOR_Renamed = (1 << 4),           /**< An object was renamed. */
    MONITOR_OwnerModified = (1 << 5),     /**< The owner of an object was modified. */
    MONITOR_AttributeModified = (1 << 6), /**< The attributes of an object were modified. */
    MONITOR_MovedFrom = (1 << 7),         /**< An object was moved from this location. */
    MONITOR_MovedTo = (1 << 8),           /**< An object was moved to this location. */
    MONITOR_IsFile = (1 << 9),            /**< The object is a file. */
    MONITOR_IsDir = (1 << 10),            /**< The object is a directory. */
    MONITOR_IsSymLink = (1 << 11),        /**< The object is a symbolic link. */
    MONITOR_Link = (1 << 12),             /**< The link count of an object has changed. */
    MONITOR_Overflow = (1 << 13)          /**< The event queue has overflowed. */
} MONITOR_EVE_E;

typedef struct MONITOR_FILES_TAG {
    std::string     filePath;
    uint32_t        envFlages;
    time_t          time;
} MONITOR_FILES_T;

class MonitorUtils
{

public:
    int32_t start();
    int32_t stop();
    int32_t filtrationEvents(const std::string filePath);
    int32_t filtrationEvents(event event, uint32_t &evnFlage);
    static FSW_EVENT_CALLBACK porcessEnvet;

public:
    int32_t construct();
    int32_t destruct();
    MonitorUtils(const std::vector<std::string> path, std::function<void (const std::vector<event>&)> func);
    virtual ~MonitorUtils();

private:
    MonitorUtils() = delete;
    MonitorUtils(const MonitorUtils &rhs) = delete;
    MonitorUtils &operator=(const MonitorUtils &rhs) = delete;

private:
    bool                            mConstructed;
    std::vector<std::string>        mPaths;
    monitor                         *mActiveMonitor;
    ModuleType                      mModule;
    std::vector<MONITOR_FILES_T>    mMonitorFiles;
    std::function<void (const std::vector<event>&)> mFunc;
};  /* class MonitorUtils */

}  /* namespace uranium */

#endif  // __MONITORUTILS_H__ 

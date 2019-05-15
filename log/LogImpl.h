#ifndef _LOG_IMPL_H_
#define _LOG_IMPL_H_

#include "version.h"
#include "modules.h"

namespace sirius {

#define MODULE_NAME     PROJNAME
#define DBG_LOG_MAX_LEN 256

#define DBG_ASSERT_RAISE_TRAP

enum LogType {
    LOG_TYPE_NONE,
    LOG_TYPE_DEBUG,
    LOG_TYPE_INFO,
    LOG_TYPE_WARN,
    LOG_TYPE_ERROR,
    LOG_TYPE_FATAL,
    LOG_TYPE_MAX_INVALID,
};

void __debug_log(const ModuleType module, const LogType type,
    const char *func, const int line, const char *fmt, ...);

void __assert_log(const ModuleType module, const unsigned char cond,
    const char *func, const int line, const char *fmt, ...);

extern int8_t gDebugController[][LOG_TYPE_MAX_INVALID + 1];

#undef PLOGx
#define PLOGx(module, level, fmt, args...)      \
    do {                                        \
        if (gDebugController[module][level]) {  \
            __debug_log(module, level, __FUNCTION__, __LINE__, fmt, ##args); \
        }                                       \
    } while(0)

#undef PLOGD
#define PLOGD(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_DEBUG, fmt, ##args)
#undef PLOGI
#define PLOGI(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_INFO, fmt, ##args)
#undef PLOGW
#define PLOGW(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_WARN, fmt, ##args)
#undef PLOGE
#define PLOGE(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_ERROR, fmt, ##args)
#undef PLOGF
#define PLOGF(module, fmt, args...)                \
    PLOGx(module, LOG_TYPE_FATAL, fmt, ##args)
#undef PLOGDIF
#define PLOGDIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_DEBUG, fmt, ##args) } } while(0)
#undef PLOGIIF
#define PLOGIIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_INFO, fmt, ##args) } } while(0)
#undef PLOGWIF
#define PLOGWIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_WARN, fmt, ##args) } } while(0)
#undef PLOGEIF
#define PLOGEIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_ERROR, fmt, ##args) } } while(0)
#undef PLOGFIF
#define PLOGFIF(module, cond, fmt, args...)        \
    do { if (cond) { PLOGx(module, LOG_TYPE_FATAL, fmt, ##args) } } while(0)

#undef ASSERT_PLOG
#define ASSERT_PLOG(module, cond, fmt, args...)     \
    __assert_log(module, cond, __FUNCTION__, __LINE__, fmt, ##args)

};

#endif


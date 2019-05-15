#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "LogImpl.h"

//#define SAVE_FILE_FS

#ifdef DBG_ASSERT_RAISE_TRAP
#include "signal.h"
#endif

#ifdef PRINT_LOGCAT_LOG
#include <utils/Log.h>

#undef LOG_TAG
#define LOG_TAG PROJNAME VERSION
#endif

namespace sirius {

#define MAX_PROCESS_NAME_LEN 16
#define LOG_FILE_PATH        "/data/vendor/camera/sirius.log"
#define LOG_FILE_PATH_LAST   "/data/vendor/camera/sirius.last.log"
#define LOG_MAX_LEN_PER_LINE 10240 // Bytes

int8_t gDebugController[][LOG_TYPE_MAX_INVALID + 1] = {
    // NONE,  DBG,  INF, WARN,  ERR, FATA, INVA
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_OTHERS
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SIRIUS
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SIRIUS_IMPL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SIRIUS_CORE
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_SERVER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_SERVER_SM
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_CLIENT
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_CLIENT_SM
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SERVER_CLIENT_CONTROL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_BUF_MANAGER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_ION_BUF_MANAGER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SERVER_CB_THREAD
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_RUN_ONCE_THREAD
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_THREAD_POOL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_MEMORY_POOL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_REQUEST_HANDLER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_REQUEST_CLIENT_HANDLER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SIRIUS_CLIENT_CORE
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_PREVIEW_REQUEST
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_YUV_PICTURE_REQUEST
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_BAYER_PICTURE_REQUEST
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_EVT_REQUEST,
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_DATA_REQUEST,
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_OBJECT_BUFFER,
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_UTILS
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_TOOLS
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SERVER_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CLIENT_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_MAX_INVALID
};

static const char *const gLogType[] = {
    [LOG_TYPE_NONE]        = "<NONE>",
    [LOG_TYPE_DEBUG]       = "< DBG>",
    [LOG_TYPE_INFO]        = "< INF>",
    [LOG_TYPE_WARN]        = "<WARN>",
    [LOG_TYPE_ERROR]       = "< ERR>",
    [LOG_TYPE_FATAL]       = "<FATA>",
    [LOG_TYPE_MAX_INVALID] = "<INVA>",
};

static char    gProcess[PATH_MAX] = { '\0' };
#ifdef SAVE_FILE_FS
static int32_t gLogfd = -1;
static char    gLogLine[LOG_MAX_LEN_PER_LINE];
static pthread_mutex_t gWriteLock = PTHREAD_MUTEX_INITIALIZER;
#endif

static int32_t getMaxInvalidId(char *text, int32_t len)
{
    int32_t i = 0;

    for (; i < len; i++) {
        if (!((text[i] == '/') ||
            (text[i] == '.') || (text[i] == '_') ||
            (text[i] >= '0' && text[i] <= '9') ||
            (text[i] >= 'A' && text[i] <= 'Z') ||
            (text[i] >= 'a' && text[i] <= 'z'))) {
            text[i + 1] = '\0';
            break;
        }
    }

    return i;
}

static char *getProcessName()
{
    if (gProcess[0] == '\0') {
        pid_t pid = getpid();
        char path[32];
        char text[PATH_MAX] = { '\0' };
        int  length;

        sprintf(path, "/proc/%d/cmdline", pid);
        int32_t fd = open(path, O_RDONLY);
        if (fd > 0) {
            ssize_t len = read(fd, text, PATH_MAX);
            if (len > 0) {
                text[len] = text[getMaxInvalidId(text, len)] = '\0';
                char *index = strrchr(text, '/');
                if (index != NULL) {
                    strcpy(gProcess, index + 1);
                }
            }
            close(fd);
        }

        if (gProcess[0] == '\0') {
            text[0] = '\0';
            strcpy(path, "/proc/self/exe");
            ssize_t len = readlink(path, text, PATH_MAX);
            if (len > 0) {
                text[len] = text[getMaxInvalidId(text, len)] = '\0';
                char *index = strrchr(text, '/');
                if (index != NULL) {
                    strcpy(gProcess, index + 1);
                }
            }
        }

        if (gProcess[0] == '\0') {
            strcpy(gProcess, "Unknown");
        }

        length = strlen(gProcess);
        if (length > MAX_PROCESS_NAME_LEN) {
            memmove(gProcess,
                gProcess + length + 1 - MAX_PROCESS_NAME_LEN,
                MAX_PROCESS_NAME_LEN + 1);
        }
    }

    return gProcess;
}

static bool checkValid(LogType type)
{
    bool rc = false;

    if (type >= 0 && type < LOG_TYPE_MAX_INVALID) {
        rc = true;
    }

    return rc;
}

static LogType getValidType(LogType type)
{
    return checkValid(type) ? type : LOG_TYPE_MAX_INVALID;
}

static const char *getLogType(LogType type)
{
    return gLogType[getValidType(type)];
}

static int32_t __log_vsnprintf(char* pdst, int32_t size,
    const char* pfmt, va_list argptr)
{
    int32_t written = 0;

    pdst[0] = '\0';
    written = vsnprintf(pdst, size, pfmt, argptr);

    if ((written >= size) && (size > 0)) {
       // Message length exceeds the buffer limit size
       written = size - 1;
       pdst[size - 1] = '\0';
    }

    return written;
}

static void print_log(const LogType logt, const char *fmt,
    char *process, const char *module, const char *type,
    const char *func, const int line, const char *buf);

static void save_log(const char *fmt, char *process,
    const char *module, const char *type,
    const char *func, const int line, const char *buf);

void __debug_log(const ModuleType module, const LogType type,
    const char *func, const int line, const char *fmt, ...)
{
    char    buf[DBG_LOG_MAX_LEN];
    va_list args;

    va_start(args, fmt);
    __log_vsnprintf(buf, DBG_LOG_MAX_LEN, fmt, args);
    va_end(args);

#ifdef PRINT_LOGCAT_LOG
    print_log(type, "%s %s%s: %s:+%d: %s",
        getProcessName(), getModuleShortName(module),
        getLogType(type), func, line, buf);
#else
    print_log(type, "%s %s%s: %s:+%d: %s\n",
        getProcessName(), getModuleShortName(module),
        getLogType(type), func, line, buf);
#endif

    save_log("%s %s%s: %s:+%d: %s", getProcessName(),
        getModuleShortName(module),
        getLogType(type), func, line, buf);
}

void __assert_log(const ModuleType module, const unsigned char cond,
    const char *func, const int line, const char *fmt, ...)
{
    char    buf[DBG_LOG_MAX_LEN];
    va_list args;

    if (cond == 0) {
        va_start(args, fmt);
        __log_vsnprintf(buf, DBG_LOG_MAX_LEN, fmt, args);
        va_end(args);

        print_log(LOG_TYPE_ERROR, "[<! ASSERT !>]%s %s%s: %s:+%d: %s",
            getProcessName(), getModuleShortName(module),
            "<ASSERT>", func, line, buf);

        save_log("[<! ASSERT !>]%s %s%s: %s:+%d: %s",
            getProcessName(), getModuleShortName(module),
            "<ASSERT>", func, line, buf);

#ifdef DBG_ASSERT_RAISE_TRAP
        save_log("[<! ASSERT !>] Process will suicide now.",
            getProcessName(), getModuleShortName(MODULE_OTHERS),
            getLogType(LOG_TYPE_FATAL), __FUNCTION__, __LINE__, buf);
        raise(SIGTRAP);
#endif
    }
}

#ifdef SAVE_FILE_FS
static void save_log(const char *fmt, char *process,
    const char *module, const char *type,
    const char *func, const int line, const char *buf)
{
    if (gLogfd == -1) {
        if (access(LOG_FILE_PATH_LAST, F_OK) ||
            unlink(LOG_FILE_PATH_LAST)) {
            print_log(LOG_TYPE_ERROR, fmt,
                process, getModuleShortName(MODULE_OTHERS),
                getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                "Failed to remove last log file " LOG_FILE_PATH_LAST);
        }
    }

    if (gLogfd == -1) {
        if (!access(LOG_FILE_PATH, F_OK)) {
            if (rename(LOG_FILE_PATH, LOG_FILE_PATH_LAST)) {
                print_log(LOG_TYPE_ERROR, fmt,
                    process, getModuleShortName(MODULE_OTHERS),
                    getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                    "Failed to rename log file " LOG_FILE_PATH
                    " to " LOG_FILE_PATH_LAST);
            }
        }
    }

    if (gLogfd == -1) {
        if (access(LOG_FILE_PATH, F_OK) ||
            unlink(LOG_FILE_PATH)) {
            print_log(LOG_TYPE_ERROR, fmt,
                process, getModuleShortName(MODULE_OTHERS),
                getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                "Failed to remove curr log file " LOG_FILE_PATH);
        }
    }

    if (gLogfd == -1) {
        gLogfd = open(LOG_FILE_PATH, O_RDWR | O_CREAT | O_TRUNC, 0777);
        if (gLogfd < 0) {
            print_log(LOG_TYPE_ERROR, fmt,
                process, getModuleShortName(MODULE_OTHERS),
                getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__,
                "Failed to create file " LOG_FILE_PATH " for logs.");
        }
    }

    if (gLogfd > 0) {
        time_t t  = time(NULL);
        struct tm* local = localtime(&t);
        char timeBuf[32];
        strftime(timeBuf, sizeof(timeBuf) - 1, "%Y-%m-%d %H:%M:%S", local);
        struct timeval tv;
        gettimeofday(&tv, NULL);

        pthread_mutex_lock(&gWriteLock);
        snprintf(gLogLine, sizeof(gLogLine) - 1, "%s.%03ld pid %d tid %ld ",
            timeBuf, tv.tv_usec / 1000, getpid(), pthread_self());
        int32_t cnt = strlen(gLogLine);
        snprintf(gLogLine + cnt, sizeof(gLogLine) - cnt - 1,
            fmt, process, module,
            type, func, line, buf);
        cnt = strlen(gLogLine);
        gLogLine[cnt++] = '\n';
        ssize_t len = write(gLogfd, gLogLine, cnt);
        if (cnt > len) {
            char tmp[255];
            sprintf(tmp, "Log len %d bytes, written %d bytes.",
                cnt, len);
            print_log(LOG_TYPE_ERROR, fmt,
                process, getModuleShortName(MODULE_OTHERS),
                getLogType(LOG_TYPE_ERROR), __FUNCTION__, __LINE__, tmp);
        }
        pthread_mutex_unlock(&gWriteLock);
    }
}
#else
static void save_log(const char * /*fmt*/, char * /*process*/,
    const char * /*module*/, const char * /*type*/,
    const char * /*func*/, const int /*line*/, const char * /*buf*/)
{
}
#endif

static void print_log(const LogType logt __attribute__((unused)),
    const char *fmt, char *process, const char *module, const char *type,
    const char *func, const int line, const char *buf)
{
#ifdef PRINT_LOGCAT_LOG
    switch (logt) {
        case LOG_TYPE_NONE:
        case LOG_TYPE_DEBUG:
            ALOGD(fmt, process, module, type, func, line, buf);
            break;
        case LOG_TYPE_INFO:
            ALOGI(fmt, process, module, type, func, line, buf);
            break;
        case LOG_TYPE_WARN:
            ALOGW(fmt, process, module, type, func, line, buf);
            break;
        case LOG_TYPE_ERROR:
        case LOG_TYPE_FATAL:
            ALOGE(fmt, process, module, type, func, line, buf);
            break;
        case LOG_TYPE_MAX_INVALID:
        default:
            ALOGE(fmt, process, module, type, func, line, buf);
            break;
    }
#else
    printf(fmt, process, module, type, func, line, buf);
#endif
}

};


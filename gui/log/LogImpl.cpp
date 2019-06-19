#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>

#include <QApplication>

#include "LogImpl.h"

namespace uranium {

#define MAX_PROCESS_NAME_LEN 16
#define LOG_FILE_PATH        "uranium-gui.log"
#define LOG_FILE_PATH_LAST   "uranium-gui.last.log"
#define LOG_MAX_LEN_PER_LINE 10240 // Bytes

int8_t gDebugController[][LOG_TYPE_MAX_INVALID + 1] = {
    // NONE,  DBG,  INF, WARN,  ERR, FATA, INVA
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_OTHERS
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CORE
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CORE_IMPL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_GUI
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_IPC
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_AES
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_AES_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_ENCRYPT
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_ENCRYPT_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_UUID
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_UUID_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SHA1
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SHA1_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_FILE
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_FILE_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_DIR
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_DIR_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_TRANSMITION
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_TRANSMITION_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_THREAD_POOL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_THREAD_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_MEMORY_POOL
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_MEMORY_TESTER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_SERVER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_SERVER_SM
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_CLIENT
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_SOCKET_CLIENT_SM
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_OBJECT_BUFFER
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_CONFIGURATION
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_UTILS
    {     0,    1,    1,    1,    1,    1,    0}, // MODULE_TOOLS
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

static int32_t gLogfd = -1;
static char    gLogLine[LOG_MAX_LEN_PER_LINE];
static pthread_mutex_t gWriteLock = PTHREAD_MUTEX_INITIALIZER;

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
    strcpy(gProcess, PROJNAME);
    strcat(gProcess, " gui");

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

    print_log(type, "%s %s%s: %s:+%d: %s\n",
        getProcessName(), getModuleShortName(module),
        getLogType(type), func, line, buf);

    save_log("%s %s%s: %s:+%d: %s\n", getProcessName(),
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

        print_log(LOG_TYPE_ERROR, "[<! ASSERT !>]%s %s%s: %s:+%d: %s\n",
            getProcessName(), getModuleShortName(module),
            "<ASSERT>", func, line, buf);

        save_log("[<! ASSERT !>]%s %s%s: %s:+%d: %s\n",
            getProcessName(), getModuleShortName(module),
            "<ASSERT>", func, line, buf);

        save_log("[<! ASSERT !>] Process will suicide now.\n",
            getProcessName(), getModuleShortName(MODULE_OTHERS),
            getLogType(LOG_TYPE_FATAL), __FUNCTION__, __LINE__, buf);

        QCoreApplication::instance()->exit(1);
    }
}

extern int64_t getThreadId();

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
        snprintf(gLogLine, sizeof(gLogLine) - 1, "%s.%03ld pid %d tid unknown ",
            timeBuf, tv.tv_usec / 1000, getpid());
        int32_t cnt = strlen(gLogLine);
        snprintf(gLogLine + cnt, sizeof(gLogLine) - cnt - 1,
            fmt, process, module,
            type, func, line, buf);
        cnt = strlen(gLogLine);
        gLogLine[cnt++] = '\n';
        int32_t len = write(gLogfd, gLogLine, cnt);
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

static void print_log(const LogType logt __attribute__((unused)),
    const char *fmt, char *process, const char *module, const char *type,
    const char *func, const int line, const char *buf)
{
    printf(fmt, process, module, type, func, line, buf);
}

};


#ifndef __MONITORCORE_H__
#define __MONITORCORE_H__
#include <functional>
#include <pthread.h>

#include "common.h"
#include "threads/ThreadPoolEx.h"
#include "MonitorUtils.h"
#include "FileManager.h"

namespace uranium
{
class MonitorUtils;
class FileManager;
class MonitorCore
{
public:
    bool monitorDirNotExit(void);
    bool monitorDirCompareWithLocal(const std::string file, \
                                    std::map<std::string, uint32_t> &diffFile);
    int32_t monitorTarExec(const std::string &files, const std::string &dirPath, std::function<int32_t (void)> cb);
    int32_t monitorUntarExec(const std::string &files, const std::string &dirPath, std::function<int32_t (void)> cb);
    int32_t monitorDirInfosSave(const std::string path, std::function<int32_t (void)> cb);
    int32_t monitorDirInfosLoad(const std::string path);
    int32_t monitorDirInfosScan();

public:
    int32_t monitorDirStart(void);
    int32_t monitorDirStop(void);

public:
    int32_t monitorLoopProcess(std::function<int32_t (std::map<std::string, uint32_t>&) > cb);
    int32_t monitorLoopStop(void);

public:
    int32_t construct();
    int32_t destruct();
    MonitorCore(std::string monitorPath);
    virtual ~MonitorCore();

private:
    MonitorCore() = delete;
    MonitorCore(const MonitorCore &rhs) = delete;
    MonitorCore &operator=(const MonitorCore &rhs) = delete;
    void processHandle(const std::vector<event>& events);

private:
    bool            mConstructed;
    bool            mLoopRuning;
    bool            mMoniStarFlag;
    ModuleType      mModule;
    MonitorUtils    *mMonitor;
    FileManager     *mFileMage;
    // std::string     mFileMagePath;
    std::string     mMonitorPath;
    ThreadPoolEx    *mThreads;
    ThreadPoolEx    *mLoopThreads;
    std::map<std::string, MONITOR_FILES_T> mFileModify;
    pthread_mutex_t  infoMutex;

};  // class MonitorCore

}

#endif //__MONITORCORE_H__

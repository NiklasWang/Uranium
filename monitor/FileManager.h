/**
 * @file FileManager.h
 * @brief
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-06-04
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */
#ifndef __FILE_MANAGER_H__
#define __FILE_MANAGER_H__

#include <string>
#include <map>

#include "common.h"
#include "inicpp.h"
#include "MonitorUtils.h"
#include "threads/ThreadPoolEx.h"

namespace uranium
{

#define FILE_MANAGER_DEFAULT_NAME   "fileManager.ini"
#define FILE_MANAGER_SECTION        "FileManager"

typedef struct FILEINFOS_TAG {
    uint32_t    checksum[4];
} FILEINFOS_T;

class FileManager
{
public:
    FileManager();
    ~FileManager();
    int32_t fileTarFromPath(const std::string fromPath, const std::string compreFile);
    int32_t fileUntarToPath(const std::string compreFile, const std::string toPath);
    int32_t fileInfosSave(const std::string path = NULL);
    int32_t fileInfosLoad(const std::string path = NULL);
    int32_t fileScanToInis(const std::string path = NULL);
    int32_t filePathSet(const std::string path);
    //int32_t fileStart
private:
    int32_t startMonitorLoop();

private:
    int32_t fileInfoErase(void);
    ModuleType      mModule;
    std::string     mfilePath;
    std::string     mInfoPath;
    ini::IniFile    mFile;
    std::map<std::string, std::string> mFileInfos;
    uint32_t        myChecksum[4];
    MonitorUtils    mMonitor;
    ThreadPoolEx    *mThreads;
    bool            mRuning;
};

};
#endif // __FILE_MANAGER_H__

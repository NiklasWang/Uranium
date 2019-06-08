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

typedef struct FILEINFOS_TAG {
    uint32_t    checksum[4];
} FILEINFOS_T;

class FileManager
{
public:
    int32_t fileTarFromPath(const std::string compreFile);
    int32_t fileUntarToPath(const std::string compreFile);
    int32_t fileInfosSave(const std::string path = NULL);
    int32_t fileInfosLoad(const std::string path = NULL);
    int32_t fileScanToInis();
    bool    dirCompareWithLocal(const std::string file);
    bool    dirNotExit(void);
    //
    //int32_t filePathSet(const std::string path);
    //int32_t fileStart

public:
    int32_t construct();
    int32_t destruct();
    FileManager(const std::string storageFilePath = FILE_MANAGER_DEFAULT_NAME);
    virtual ~FileManager();

private:
    int32_t fileScanToInis(const std::string path);
    int32_t fileInfoErase(void);

private:
    FileManager() = delete;
    FileManager(const FileManager &rhs) = delete;
    FileManager &operator=(const FileManager &rhs) = delete;

private:
    ModuleType      mModule;
    std::string     mDirPath;
    std::string     mInfoPath;
    std::map<std::string, std::string> mFileInfos;
    // bool            mRuning;
};

};
#endif // __FILE_MANAGER_H__

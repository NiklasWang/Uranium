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
    bool dirCompareWithLocal(const std::string file, \
                             std::map<std::string, uint32_t> &diffFile);
    bool    dirNotExit(void);

public:
    int32_t construct();
    int32_t destruct();
    FileManager(const std::string &monitPath);
    virtual ~FileManager();

private:
    int32_t unmatched_inclusions_warn(struct archive *matching, const char *msg);
    int32_t compressWriteEntry(struct archive *disk, struct archive *writer, struct archive_entry *entry, const char *buff, size_t buff_size);
    int32_t Compress_write_hierarchy(struct archive *disk, struct archive *writer, struct archive_entry_linkresolver *resolver,
                                     const char *path, const char *buff, size_t buff_size);
    int32_t compressFile2Disk(const char *tar_file, const char *file1, ...);
    int32_t uncompressFil2Disk(const char *file, const char *to_path);

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
    std::map<std::string, std::string> mFileInfos;

};

};
#endif // __FILE_MANAGER_H__

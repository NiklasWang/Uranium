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
    int32_t folder_mkdirs(const char *folder_path);
    int32_t owner_tarFile(const std::string& filename, FILE* fpOut);
    int32_t owner_tarDIr(const std::string& dirname, FILE* fpOut);
    int32_t owner_unTarFile(FILE *fin);
    size_t owner_fileSize(const char *filename);
    int32_t owner_fileHeadErase(void);

private:
    int32_t owner_tar(const char *outfile);
    int32_t owner_unTar(const char* tarfile);
    int32_t owner_decompressFile(const char *infilename, const  char *outfilename);
    int32_t owner_compressFile(const char *infilename, const char *outfilename);

private:
    int32_t bsdTar(bool compress, std::string filePath);
    int32_t bsdUnTar(bool compress, std::string filePath);
    int32_t unmatched_inclusions_warn(struct archive *matching, const char *msg);
    int32_t compressWriteEntry(struct archive *disk, struct archive *writer, struct archive_entry *entry, char *buff, size_t buff_size);
    int32_t Compress_write_hierarchy(struct archive *disk, struct archive *writer, struct archive_entry_linkresolver *resolver,
                                     const char *path, char *buff, size_t buff_size);
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
    std::map<std::string, ino_t> mFileHead;
    uint32_t        mFileCount;

};

};
#endif // __FILE_MANAGER_H__

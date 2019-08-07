#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#include <libfswatch/archive.h>
#include <libfswatch/archive_entry.h>
#include "common.h"
#include "FileManager.h"
#include "md5.h"
#include "sha.h"
#include <stdarg.h>
#include <Exbsdtar.h>

namespace uranium
{
#define DEFAULT_BYTES_PER_BLOCK             (10240)
#define SECURITY                    \
    (ARCHIVE_EXTRACT_SECURE_SYMLINKS        \
     | ARCHIVE_EXTRACT_SECURE_NODOTDOT)

int32_t FileManager::construct()
{
    int32_t rc = NO_ERROR;
    LOGD(mModule, "construct");
    return rc;
}

int32_t FileManager::destruct()
{
    int32_t rc = NO_ERROR;
    fileInfoErase();
    LOGD(mModule, "destruct");
    return rc;
}

int32_t FileManager::bsdTar(bool compress, std::string filePath)
{
    char *argvp[4];
    char *pwdPath = NULL;
    int32_t argc = 0;

    char *buf0 = (char*) "./test";
    char *buf1 = (char*) "-cf";
    char *buf2 = (char*) filePath.c_str();
    char *buf3 = (char*) "./test";
    pwdPath = (char*) malloc(64);
    memset(pwdPath, 0, sizeof(pwdPath));
    getcwd(pwdPath, 1024);
    printf("Get pwdPath=%s\n", pwdPath);
    chdir(mDirPath.c_str());

    argvp[0] = buf0;
    argvp[1] = buf1;
    argvp[2] = buf2;
    argvp[3] = buf3;
    Exbsdtar(4, argvp);
    printf("LHB %s \n", pwdPath);
    chdir(pwdPath);
    free(pwdPath);

    return NO_ERROR;
}

int32_t FileManager::bsdUnTar(bool compress, std::string filePath)
{
    char *argvp[6];
    // char *pwdPath = NULL;
    int32_t argc = 0;

    char *buf0 = (char*) "./test";
    char *buf1 = (char*) "-jxf ";
    // char *buf2 = (char*) filePath.c_str();
    char *buf3 = (char*) " -C ";
    // char *buf4 =  mDirPath.c_str();


    argvp[0] = buf0;
    argvp[1] = buf1;
    argvp[2] = (char *) filePath.c_str();
    argvp[3] = buf3;
    argvp[4] = (char *) mDirPath.c_str();
    LOGE(mModule, "start rungint path=%s pwd=%s\n", argvp[2], argvp[4]);
    Exbsdtar(3, argvp);
    LOGE(mModule, "start run end\n");

    return NO_ERROR;
}

int32_t FileManager::unmatched_inclusions_warn(struct archive *matching, const char *msg)
{
    const char *p = NULL;
    int32_t r = 0;

    // ITE_CHECK_ERROR(NULL == matching, 0, return_err, "Invalid argument!");
    while ((r = archive_match_path_unmatched_inclusions_next(
                    matching, &p)) == ARCHIVE_OK) {
        LOGE(mModule, "%s: %s", p, msg);
    }

    if (r == ARCHIVE_FATAL) {
        LOGE(mModule, "Out of memory!");
    }
    // ITE_CHECK_ERROR((r == ARCHIVE_FATAL), 1, return_err, " Out of memory!\n");
    if (r == ARCHIVE_FATAL) {
        LOGE(mModule, "Out of memory\n");
        return NO_MEMORY;
    } else {
        return (archive_match_path_unmatched_inclusions(matching));
    }

}

int32_t FileManager::Compress_write_hierarchy(struct archive *disk, struct archive *writer, struct archive_entry_linkresolver *resolver,
        const char *path, char *buff, size_t buff_size)
{
    int32_t rc = NO_ERROR;
    struct archive_entry *entry = NULL, *spare_entry = NULL;
    int32_t ret = 0;
    if (ISNULL(disk) || ISNULL(writer) || ISNULL(resolver) || ISNULL(path) || ISNULL(buff)) {
        rc = PARAM_INVALID;
        LOGE(mModule, "param invilid\n");
    }

    if (SUCCEED(rc)) {
        ret = archive_read_disk_open(disk, path);
        if (ret != ARCHIVE_OK) {
            rc = NOT_READY;
            LOGE(mModule, "Archive_read_disk_open file error: %s!", archive_error_string(disk));
        }
    }

    if (SUCCEED(rc)) {
        for (;;) {
            printf("LHB.....\n");
            archive_entry_free(entry);
            entry = archive_entry_new();
            ret =  archive_read_next_header2(disk, entry);
            if (ret == ARCHIVE_EOF) {
                break;
            } else if (ret != ARCHIVE_OK) {
                if (ret == ARCHIVE_FATAL || ret == ARCHIVE_FAILED) {
                    rc = UNKNOWN_ERROR;
                    archive_entry_free(entry);
                    archive_read_close(disk);
                    LOGE(mModule, "Archive_read_next_header2 %s\n", archive_error_string(disk));
                    return UNKNOWN_ERROR;
                } else if (ret < ARCHIVE_WARN) {
                    continue;
                }
            }

            if (geteuid() >= 0) {
                archive_entry_set_uid(entry, geteuid());
                archive_entry_set_uname(entry, archive_read_disk_uname(disk, geteuid()));
            }

            /* Non-regular files get archived with zero size. */
            if (archive_entry_filetype(entry) != AE_IFREG) {
                archive_entry_set_size(entry, 0);
            }

            archive_entry_linkify(resolver, &entry, &spare_entry);

            while (entry != NULL) {
                ret =  compressWriteEntry(disk, writer, entry, buff, buff_size);
                if (FAILED(ret)) {
                    rc = UNKNOWN_ERROR;
                    LOGE(mModule, "Compress_write_entry failed!");
                    goto return_err;
                }
                archive_entry_free(entry);
                entry = spare_entry;
                spare_entry = NULL;
            } // end while entry != NULL
        }
    }

return_err:
    if (NOTNULL(entry)) {
        archive_entry_free(entry);
    }

    archive_read_close(disk);
    LOGD(mModule, "rc runing\n");
    return rc;

}

int32_t FileManager::compressWriteEntry(struct archive *disk, struct archive *writer, struct archive_entry *entry, char *buff, size_t buff_size)
{
    size_t   bytes_read = 0;
    ssize_t bytes_written = 0;
    int64_t offset = 0, progress = 0;
    char *null_buff = NULL;
    const void *buff_tmp;
    int32_t ret = 0;

    ret  =  archive_write_header(writer, entry);
    if ((ret >= ARCHIVE_OK) && (archive_entry_size(entry) > 0)) {
        while ((ret = archive_read_data_block(disk, &buff_tmp, &bytes_read, &offset)) == ARCHIVE_OK) {
            if (offset > progress) {
                int64_t sparse = offset - progress;
                size_t ns;
                if (null_buff == NULL) {
                    null_buff = buff;
                    memset((void*) null_buff, 0, buff_size);
                }
                while (sparse > 0) {
                    if (sparse > (int64_t)buff_size) {
                        ns = buff_size;
                    } else {
                        ns = (size_t)sparse;
                    }
                    bytes_written = archive_write_data(writer, null_buff, ns);
                    // ITE_CHECK_ERROR(ret < 0, 3, return_err, "Archive_write_data %s \n", archive_error_string(writer));
                    if ((size_t)bytes_written < ns) {
                        /* Write was truncated; warn but continue */
                        LOGD(mModule, "%s Truncated write; file may "
                             "have grown while being archived.",
                             archive_entry_pathname(entry));
                        return 0;
                    }
                    progress += bytes_written;
                    sparse -= bytes_written;
                } // end while sparse>0
            } //end if offset > progress
            bytes_written = archive_write_data(writer, buff_tmp, bytes_read);
            // ITE_CHECK_ERROR(bytes_written < 0, 3, return_err, "Archive_write_data %s\n", archive_error_string(writer));
            if ((size_t)bytes_written < bytes_read) {
                /* Write was truncated; warn but continue */
                LOGD(mModule, "%s Truncated write; file may "
                     "have grown while being archived.",
                     archive_entry_pathname(entry));
                return 0;
            }
            progress += bytes_written;
        }
        if (ret < ARCHIVE_WARN) {
            LOGE(mModule,  "Archive_read_data failed Error:%s\n", archive_error_string(writer));
            return UNKNOWN_ERROR;
        }
    }

    return NO_ERROR;
}

int32_t FileManager::uncompressFil2Disk(const char *file, const char *to_path)
{
    int32_t rc = NO_ERROR;
    int32_t ret = 0;
    int32_t extract_flags = 0;
    struct archive *matching = NULL;
    struct archive *writer = NULL;
    struct archive *reader = NULL;
    const char *p = NULL;
    struct archive_entry *entry = NULL;
    int fd = 0;
    // uint32_t file_size = 0;

    if (SUCCEED(rc)) {
        /* root 用户使用*/
        if (0 == geteuid()) {
            /* --same-owner */
            extract_flags |= ARCHIVE_EXTRACT_OWNER;
            /* -p */
            extract_flags |= ARCHIVE_EXTRACT_PERM;
            extract_flags |= ARCHIVE_EXTRACT_ACL;
            extract_flags |= ARCHIVE_EXTRACT_XATTR;
            extract_flags |= ARCHIVE_EXTRACT_FFLAGS;
            extract_flags |= ARCHIVE_EXTRACT_MAC_METADATA;
        }
    }

    if (SUCCEED(rc)) {
        matching = archive_match_new();
        if (ISNULL(matching)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Runing archive_match_new() failed out of memory\n");
        }
    }

    if (SUCCEED(rc)) {
        /* 创建writer */
        writer = archive_write_disk_new();
        if (ISNULL(writer)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Create disk writer failed");
        } else {
            archive_write_disk_set_standard_lookup(writer);
            archive_write_disk_set_options(writer, extract_flags);
        }
    }

    if (SUCCEED(rc)) {
        /* 创举reader */
        reader = archive_read_new();
        if (ISNULL(reader)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Create reader failed!");
        } else {
            /* 支持所有解压缩模式 */
            archive_read_support_filter_all(reader);
            archive_read_support_format_all(reader);
        }
    }
#if 1
    if (SUCCEED(rc)) {
        /* 读取文件总长度 */
        fd = open(file, O_RDWR);
        if (fd < 0) {
            rc = NOT_READY;
            LOGE(mModule, "Open %s failed with %s\n", file, strerror(errno));
        } else {
            // close(fd);
        }
    }
#endif
    if (SUCCEED(rc)) {
        /* 打开文件 */
        ret = archive_read_open_fd(reader, fd, DEFAULT_BYTES_PER_BLOCK);
        // ret = archive_read_open_filename(reader, file, DEFAULT_BYTES_PER_BLOCK);
        if (FAILED(ret)) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Archive read open file = %s fd=%d %s failed!\n", file, fd, archive_error_string(reader));
        }
    }

    if (SUCCEED(rc)) {
        /* 设置解压路径 */
        if (to_path) {
            ret = chdir(to_path);
            if (FAILED(ret)) {
                rc = UNKNOWN_ERROR;
                LOGE(mModule, "Set direction failed!\n");
            }
        }
    }
#if 0
    if (SUCCEED(rc)) {
        /* 设置解压进行回调函数 */
        g_progress_data = malloc(sizeof(ITE_TAR_COMPRESS_DATA_T));

        ITE_CHECK_ERROR(NULL == g_progress_data, 7, return_err, "Memeory is not enough!\n");
        memset(g_progress_data, 0, sizeof(ITE_TAR_COMPRESS_DATA_T));
        g_progress_data->archive = reader;
        g_progress_data->file_size = file_size;
        /* 文件操作加锁 */
        archive_read_extract_set_progress_callback(reader, progress_func, g_progress_data);
    }
#endif
    if (SUCCEED(rc)) {
        /* 解压流程 */
        for (;;) {
            ret = archive_match_path_unmatched_inclusions(matching);

            ret = archive_read_next_header(reader, &entry);
            /* 检查返回结果 */
            if (ret == ARCHIVE_EOF) {
                rc = NO_ERROR;
                break;
            }

            if (ret < ARCHIVE_OK) {
                LOGE(mModule, "%s\n", archive_error_string(reader));
            }
            /*  Retryable error: try again  */
            if (ret == ARCHIVE_RETRY) {
                continue;
            }

            if (ret == ARCHIVE_FATAL) {
                ret = UNKNOWN_ERROR;
                break;
            }

            p = archive_entry_pathname(entry);
            if (p == NULL || p[0] == '\0') {
                LOGE(mModule, "Archvie entry has empty or unreadable filename ....skipping.\n");
                continue;
            }
            /* 设置用户ID*/
            archive_entry_set_uid(entry, 0);
            archive_entry_set_uname(entry, NULL);

            if (archive_match_excluded(matching, entry)) {
                continue; /* Excluded by a pattern test. */
            }

            /* 写入disk */
            ret =  archive_read_extract2(reader, entry, writer);
            if (ret != ARCHIVE_OK) {
                rc = UNKNOWN_ERROR;
                break;
                LOGE(mModule, "Read To extract2 failed %s:%s\n", archive_entry_pathname(entry), archive_error_string(reader));
            }
        }
    }

    if (NOTNULL(reader)) {
        archive_read_close(reader);
        archive_write_close(reader);
        archive_read_free(reader);
    }

    if (NOTNULL(writer)) {
        archive_write_free(writer);
    }

    if (NOTNULL(matching)) {
        unmatched_inclusions_warn(matching, "Not found in archive!\n");
        archive_match_free(matching);
    }
    return rc;
}

int32_t FileManager::compressFile2Disk(const char *tar_file, const char *file1, ...)
{
    int32_t rc = NO_ERROR;
    int32_t ret = 0;
    int32_t extract_flags = 0;
    int32_t readdisk_flags = 0;
    struct archive *matching = NULL;
    struct archive *diskreader = NULL;
    struct archive *writer = NULL;
    struct archive_entry_linkresolver *resolver = NULL;
    struct archive_entry *entry = NULL, *sparse_entry = NULL;
    size_t buffer_size = 64 * 1024;
    char * buff = NULL;

    if (ISNULL(tar_file)) {
        rc = PARAM_INVALID;
    }

    if (SUCCEED(rc)) {
        /* root 用户使用*/
        if (0 == geteuid()) {
            /* --same-owner */
            extract_flags |= ARCHIVE_EXTRACT_OWNER;
            /* -p */
            extract_flags |= ARCHIVE_EXTRACT_PERM;
            extract_flags |= ARCHIVE_EXTRACT_ACL;
            extract_flags |= ARCHIVE_EXTRACT_XATTR;
            extract_flags |= ARCHIVE_EXTRACT_FFLAGS;
            extract_flags |= ARCHIVE_EXTRACT_MAC_METADATA;
        }
        /* Enable Mac OS "copyfile()" extension by default. */
        /* This has no effect on other platforms. */
        readdisk_flags = ARCHIVE_READDISK_MAC_COPYFILE;

        matching = archive_match_new();
        if (ISNULL(matching)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Out of memory match new failed");
        }
    }

    if (SUCCEED(rc)) {
        /* create writer */
        writer = archive_write_new();
        if (ISNULL(writer)) {
            rc = NO_MEMORY;
            LOGE(mModule, "create archive writer failed\n");
        } else {
            archive_write_set_format_pax_restricted(writer);
        }
    }

    if (SUCCEED(rc)) {
        /* set block sizes */
        archive_write_set_bytes_per_block(writer, DEFAULT_BYTES_PER_BLOCK);
        archive_write_set_bytes_in_last_block(writer, -1);
        /* 设置xz 压缩 */
        ret = archive_write_add_filter_by_name(writer, "bzip2");
        if (ret  < ARCHIVE_WARN) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Set compression option as 'xz' failed!");
        }
    }

    if (SUCCEED(rc)) {
        ret  = archive_write_open_filename(writer, tar_file);
        if (FAILED(ret)) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Writer open file[%s] failed\n", tar_file);
        }
    }

    if (SUCCEED(rc)) {
        while (buffer_size < (size_t) DEFAULT_BYTES_PER_BLOCK) {
            buffer_size *= 2;
        }
        /* Try to compensate for space we'll lose to alignment. */
        buffer_size += 16 * 1024;
        buff = (char*)malloc(buffer_size);
        if (ISNULL(buff)) {
            rc  = NO_MEMORY;
            LOGE(mModule, "Out of memory ");
        }
    }

    if (SUCCEED(rc)) {
        resolver = archive_entry_linkresolver_new();
        if (ISNULL(resolver)) {
            rc = NO_MEMORY;
            LOGE(mModule, "archive_entry_linkresolver_new() failed");
        } else {
            archive_entry_linkresolver_set_strategy(resolver, archive_format(writer));
        }
    }

    if (SUCCEED(rc)) {
        /* set reader point */
        diskreader = archive_read_disk_new();
        if (ISNULL(diskreader)) {
            rc = NO_MEMORY;
            LOGE(mModule, "archive_read_disk_new() failed");
        }
    }

    if (SUCCEED(rc)) {
        /* 设置软连接压缩属性 */
        archive_read_disk_set_symlink_physical(diskreader);
        /* Register entry filters. */
        /* TODO 添加excluded回调函数*/
        archive_read_disk_set_matching(diskreader, matching, NULL, NULL);
        /* TODO 设置元数据回调函数 */
        archive_read_disk_set_metadata_filter_callback(diskreader, NULL, NULL);
        /* 设置readerdisk 的行为*/
        archive_read_disk_set_behavior(diskreader, readdisk_flags);
        archive_read_disk_set_standard_lookup(diskreader);
    }

    if (SUCCEED(rc)) {
        /* 将文件或路径写入到diskreader 中*/
        ret = Compress_write_hierarchy(diskreader, writer, resolver, file1, buff, buffer_size);
        if (ret != ARCHIVE_OK) {
            rc = UNKNOWN_ERROR;
            LOGE(mModule, "Archive_read_disk_open file is %s error:%s!\n", file1,
                 archive_error_string(diskreader));
        }
    }

    if (SUCCEED(rc)) {
#if 0
        va_list argp;
        char *para = NULL;
        va_start(argp, file1);
        while (1) {
            para = va_arg(argp, char *);
            if (strcmp(para, "") == 0) { break; }
            rc = Compress_write_hierarchy(diskreader, writer, resolver, para, buff, buffer_size);
            if (FAILED(rc)) {
                rc = UNKNOWN_ERROR;
                LOGE(mModule, "Archive_read_disk_open file is %s error:%s!\n", file1,
                     archive_error_string(diskreader));
                break;
            }
        }
        va_end(argp);
#endif
        archive_read_disk_set_matching(diskreader, NULL, NULL, NULL);
        archive_read_disk_set_metadata_filter_callback(diskreader, NULL, NULL);
    }


    if (SUCCEED(rc)) {
        entry = NULL;
        archive_entry_linkify(resolver, &entry, &sparse_entry);
        while (entry != NULL) {
            struct archive_entry *entry2;
            struct archive *disk = diskreader;
            ret = archive_read_disk_open(disk, archive_entry_sourcepath(entry));
            if (ret != ARCHIVE_OK) {
                printf("%s\n", archive_error_string(disk));
                archive_entry_free(entry);
                continue;
            }

            entry2 = archive_entry_new();
            ret = archive_read_next_header2(disk, entry2);
            archive_entry_free(entry2);
            if (ret != ARCHIVE_OK) {
                printf("%s\n", archive_error_string(disk));
                if (ret != ARCHIVE_FATAL) {
                    archive_read_close(disk);
                }
                archive_entry_free(entry);
                continue;
            }

            compressWriteEntry(diskreader, writer, entry, buff, buffer_size);
            archive_entry_free(entry);
            archive_read_close(disk);
            entry = NULL;
            archive_entry_linkify(resolver, &entry, &sparse_entry);
        }
    }

    if (NOTNULL(writer)) {
        archive_write_close(writer);
        archive_write_free(writer);
    }
#if 0
    if (NOTNULL(diskreader)) {
        archive_read_free(diskreader);
    }
#endif
    if (NOTNULL(resolver)) {
        archive_entry_linkresolver_free(resolver);
    }

    if (NOTNULL(buff)) {
        free(buff);
    }

    if (NOTNULL(matching)) {
        archive_match_free(matching);
    }
    return rc;
}
int32_t FileManager::fileTarFromPath(const std::string compreFile)
{
    int32_t rc = 0;
    std::string tmpStr = mDirPath;

    if (SUCCEED(rc)) {
        /* exam file and path is exited */
        rc = access(tmpStr.c_str(), F_OK);
        if (FAILED(rc)) {
            LOGE(mModule, "File/Path %s not exit\n", tmpStr.c_str());
        }
    }

    if (SUCCEED(rc)) {
        bsdTar(true, compreFile);
        // rc = compressFile2Disk(compreFile.c_str(), "/home/binson/libarchive");
#if 0
        std::string cmd = ("cd ");
        cmd += tmpStr;
        cmd += ";tar -jcf ";
        cmd += compreFile;
        cmd += " *";

        rc = system(cmd.c_str());
#endif
        if (FAILED(rc)) {
            LOGE(mModule, "compressFile2Disk() failed!\n");
        }
    }

    return rc;
}

int32_t FileManager::fileUntarToPath(const std::string compreFile)
{
    int32_t rc = 0;
    std::string tmpStr = mDirPath;
    if (SUCCEED(rc)) {
        /* exam file and path is exited */
        rc = access(compreFile.c_str(), F_OK);
        if (FAILED(rc)) {
            LOGE(mModule, "File/Path %s not exit\n", compreFile.c_str());
        }
    }

    if (SUCCEED(rc)) {
        // rc = bsdUnTar(true, compreFile);
        rc = uncompressFil2Disk(compreFile.c_str(), tmpStr.c_str());
#if 0
        std::string cmd = "tar -axf ";
        cmd += compreFile + " -C " + tmpStr;
        rc = system(cmd.c_str());
#endif
        if (FAILED(rc)) {
            LOGE(mModule, "Runing uncompressFil2Disk failed!\n");
        }

    }
    return rc;
}

int32_t FileManager::fileInfosSave(const std::string path)
{
    int32_t rc = 0;
    std::string tmpStr = path;

    if (SUCCEED(rc)) {
        if (tmpStr.empty()) {
            rc = -1;
        }
    }

    if (SUCCEED(rc)) {
        std::ofstream ostream(tmpStr, std::ios::binary);
        std::map<std::string, std::string>::iterator iter;
        for (iter = mFileInfos.begin(); iter != mFileInfos.end(); iter++) {
            ostream << iter->first << "=" << mFileInfos[iter->first] << std::endl;
        }
    }

    return rc;
}

int32_t FileManager::fileInfosLoad(const std::string path)
{
    int32_t rc = 0;

    std::string tmpStr = path;

    if (SUCCEED(rc)) {
        if (tmpStr.empty()) {
            rc = -1;
        }
    }

    if (SUCCEED(rc)) {
        /* clear all trees */
        fileInfoErase();
        std::ifstream istrm(tmpStr, std::ios::binary);
        while (!istrm.eof()) {
            std::string tmpStr;
            istrm >> tmpStr;
            std::string key;
            std::string value;
            std::string::size_type point;
            point = tmpStr.find('=');
            if (point != std::string::npos) {
                key = tmpStr.substr(0, point);
                value = tmpStr.substr(point + 1);
                mFileInfos[key] = value;
            }
        }
    }

    return rc;
}

int32_t FileManager::fileScanToInis()
{
    return fileScanToInis(mDirPath);
}

bool FileManager::dirCompareWithLocal(const std::string file, \
                                      std::map<std::string, uint32_t> &diffFile)
{
    std::string tmpStr = file;
    std::map<std::string, std::string> tmpFileInfo;
    std::ifstream istrm(tmpStr, std::ios::binary);
    std::map<std::string, std::string>::iterator it;

    while (!istrm.eof()) {
        std::string ss;
        istrm >> ss;
        std::string key;
        std::string value;
        std::string::size_type point;
        point = ss.find('=');
        if (point != std::string::npos) {
            key = ss.substr(0, point);
            value = ss.substr(point + 1);
            tmpFileInfo[key] = value;
        }
    }

    /* loop compare load file infos  whit local dir scan files infos */

    for (it = tmpFileInfo.begin(); it != tmpFileInfo.end(); it++) {
        // it = mFileInfos.erase(it);
        auto result = mFileInfos.find(it->first);
        if (result != mFileInfos.end()) {
            /* find the value */
            if (result->second != it->second) {
                diffFile[it->first] = MONITOR_Updated;
            }
        } else {
            diffFile[it->first] = MONITOR_Removed;
        }
    }

    /* exam local have  removte not had */
    for (it = mFileInfos.begin(); it != mFileInfos.end(); it++) {
        auto result = tmpFileInfo.find(it->first);
        if (result == tmpFileInfo.end()) {
            diffFile[it->first] = MONITOR_Created;
        }
    }

    return false;

}

bool FileManager::dirNotExit(void)
{
    struct stat sb;
    return stat(mDirPath.c_str(), &sb);
}

int32_t FileManager::fileScanToInis(const std::string path)
{
    int32_t rc = 0;
    DIR    *dir;
    std::string thisPath = path;

    if (thisPath.empty()) {
        rc = -1;
    }
    /*  */
    if (SUCCEED(rc)) {
#ifdef __linux
        if ('/' != thisPath[thisPath.size() - 1]) {
            thisPath += "/";
        }
#endif  // __linux
        dir = opendir(thisPath.c_str());
        if (ISNULL(dir)) {
            rc = -1;
            LOGE(mModule, "opendir %s failed!", thisPath.c_str());
        }
    }

    if (SUCCEED(rc)) {
        struct    dirent    *ptr;
        while ((ptr = readdir(dir)) != NULL) { ///read the list of this dir
#ifdef __linux
            if (ptr->d_name == strchr(ptr->d_name, '.')) {
                continue;
            }

            if (DT_DIR == ptr->d_type) {
                /* this is dir */
                std::string nPath = thisPath + ptr->d_name;
                nPath += "/";
                fileScanToInis(nPath);
            } else if (DT_REG  == ptr->d_type) {
                /* this is files*/
                /* calcule md5sum */
                std::string filePath = thisPath + ptr->d_name;

                FILE *pFile = fopen(filePath.c_str(), "r");
                if (!ISNULL(pFile)) {
                    uint32_t checksum[4];
                    memset(checksum, 0, sizeof(checksum));
                    md5_stream(pFile, checksum);

                    std::stringstream sst;

                    for (int jj = 0; jj < 4; jj++) {
                        sst << std::hex << checksum[jj];
                    }
                    std::string::size_type pos = filePath.find(mDirPath);
                    pos += mDirPath.length();
                    std::string relaPath = filePath.substr(pos);
                    mFileInfos[relaPath] = sst.str();
#if 0
                    std::cout << filePath << "=" << mFileInfos[filePath] << std::endl;
#endif
                    fclose(pFile);
                }
            }
#endif  //__linux
        }
    }

    if (!ISNULL(dir)) {
        closedir(dir);
    }

    return rc;
}

int32_t FileManager::fileInfoErase(void)
{
    std::map<std::string, std::string>::iterator it;
    for (it = mFileInfos.begin(); it != mFileInfos.end();) {
        it = mFileInfos.erase(it);
    }

    return 0;
}

FileManager::FileManager(const std::string &monitPath):
    mModule(MODULE_MONITOR)
{
    std::string thisPath = monitPath;
    if ('/' != thisPath[thisPath.size() - 1]) {
        thisPath += "/";
    }

    mDirPath = thisPath;
}

FileManager::~FileManager()
{

}

}

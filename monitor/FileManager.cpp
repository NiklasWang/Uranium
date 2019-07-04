#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>

#include "common.h"
#include "FileManager.h"
#include "md5.h"
#include "sha.h"
namespace uranium
{

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
        std::string cmd = ("cd ");
        cmd += tmpStr;
        cmd += ";tar -jcf ";
        cmd += compreFile;
        cmd += " *";

        rc = system(cmd.c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "system %s failed!\n", cmd.c_str());
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
        std::string cmd = "tar -axf ";
        cmd += compreFile + " -C " + tmpStr;
        rc = system(cmd.c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "Runing system(%s) failed!\n", cmd.c_str());
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
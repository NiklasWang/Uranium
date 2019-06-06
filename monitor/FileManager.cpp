/**
 * @file FileManager.cpp
 * @brief
 * @author  lenovo <xxx@lenovo.com>
 * @version 1.0.0
 * @date 2019-06-04
 */
/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "common.h"
#include "FileManager.h"
#include "md5.h"
#include "sha.h"
namespace uranium
{

FileManager::FileManager()
{

}
FileManager::~FileManager()
{

}

int32_t FileManager::fileTarFromPath(const std::string fromPath, const std::string compreFile)
{
    int32_t rc = 0;

    if (SUCCEED(rc)) {
        /* exam file and path is exited */
        rc = access(fromPath.c_str(), F_OK);
        if (FAILED(rc)) {
            LOGE(mModule, "File/Path %s not exit\n", fromPath.c_str());
        }
    }

    if (SUCCEED(rc)) {
        std::string cmd = ("tar -jcf ");
        cmd += fromPath + " " + compreFile;
        LOGE(mModule, "run cmd =%s\n", cmd.c_str());

        rc = system(cmd.c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "system %s failed!\n", cmd.c_str());
        }
    }

    return rc;
}

int32_t FileManager::fileUntarToPath(const std::string compreFile, const std::string toPath)
{
    int32_t rc = 0;

    if (SUCCEED(rc)) {
        /* exam file and path is exited */
        rc = access(compreFile.c_str(), F_OK);
        if (FAILED(rc)) {
            LOGE(mModule, "File/Path %s not exit\n", compreFile.c_str());
        }
    }

    if (SUCCEED(rc)) {
        std::string cmd = "tar -axf ";
        cmd += compreFile + " -C " + toPath;
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

    std::map<std::string, std::string>::iterator iter;
    for (iter = mFileInfos.begin(); iter != mFileInfos.end(); iter++) {
        mFile[FILE_MANAGER_SECTION][iter->first] = mFileInfos[iter->first];

    }

    mFile.save(path.c_str());

    return rc;
}

int32_t FileManager::fileInfosLoad(const std::string path)
{
    int32_t rc = 0;
    /* clear all trees */
    fileInfoErase();
#if 0
    mFile.load(path.c_str());

    for (int32_t i = 0; i < CONFIG_MAX_INVALID; i++) {
        std::string key = whoamI(static_cast<ConfigItem>(i));
        std::string value = mFile[INI_FILE_SECTION][key.c_str()].as<std::string>();
        if (value.length()) {
            mConfigs[key] = value;
            LOGD(mModule, "Configuration found, %s=%s", key.c_str(), value.c_str());
        } else {
            rc |= NOT_FOUND;
            LOGE(mModule, "Not found %s in config file %s", key.c_str(), mFileName.c_str());
        }
    }
#endif
    return rc;
}

int32_t FileManager::fileScanToInis(const std::string path)
{
    int32_t rc = 0;
    DIR    *dir;
    std::string thisPath = path;

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
                // printf("countine\n");
                //std::cout << ptr->d_name<<std::endl;
                // ("LHB d_type:%d d_name: %s%s\n", ptr->d_type, thisPath.c_str(), ptr->d_name);
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
                // std::cout<< thisPath<< ptr->d_name<<std::endl;
                std::string filePath = thisPath + ptr->d_name;

                FILE *pFile = fopen(filePath.c_str(), "r");
                // FILE *pFile = fopen("/mnt/d/lenvov_wokspace/source/Uranium/Makefile","r");
                if (!ISNULL(pFile)) {
                    uint32_t checksum[4];
                    memset(checksum, 0, sizeof(checksum));
                    md5_stream(pFile, checksum);

                    std::stringstream sst;

                    for (int jj = 0; jj < 4; jj++) {
                        sst << std::hex << checksum[jj];
                    }
                    mFileInfos[filePath] = sst.str();
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

int32_t FileManager::filePathSet(const std::string path)
{
    mfilePath = path;
    return 0;
}

int32_t FileManager::fileInfoErase(void)
{
    std::map<std::string, std::string>::iterator it;
    for (it = mFileInfos.begin(); it != mFileInfos.end();) {
        it = mFileInfos.erase(it);
    }

    return 0;
}

}
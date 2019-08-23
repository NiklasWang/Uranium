#include <stdlib.h>
#include <iostream>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fstream>
#include "common.h"
#include "libcurl/curl/curl.h"
#include "TransferInterface.h"
#include "FexTransfer.h"
#include <algorithm>
namespace uranium
{

size_t FexTransfer::wirte_data(void*  buffer, size_t size, size_t nmemb, void *userp)
{
    uint32_t length = 2048;
    if (nmemb < length) {
        length = nmemb;
    }
    memcpy(userp, buffer, length);
    return nmemb;
}

FexTransfer::FexTransfer(TRANSFER_STATUS_E tranDirct, std::string name, std::string passWd):
    mModule(MODULE_TRANSMITION),
    mTranDirct(tranDirct),
    mName(name),
    mPassWd(passWd),
    mInitalized(false),
    mLoginStatus(false)
{
    pthread_mutex_init(&mTransMutex, NULL);
}

FexTransfer::~FexTransfer()
{
    pthread_mutex_destroy(&mTransMutex);
}

int32_t FexTransfer::construct()
{
    int32_t rc = NO_ERROR;

    if (mInitalized) {
        LOGD(mModule, "This module has initalized\n");
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        /* check login status */
        uint32_t runTimes = 2;
        do {
            rc = fexLogin();
            if (FAILED(rc)) {
                LOGE(mModule, "Fex login Failed\n");
            }
            if (fexCheckLoginStatus() == true) {
                mLoginStatus = true;
                LOGD(mModule, "Login Succeed");
                break;
            }
        } while (runTimes--);
    }

    if (SUCCEED(rc)) {
        mInitalized = true;
    }
#if 0
    std::string fileList;
    fexUploadFile("/mnt/d/lenvov_wokspace/source/Uranium/transmission/tester/SERVER.bin");
    fexFileList(fileList);
    std::cout << "fileName=" << fileList << std::endl;
    fexDownloadFile(fileList, "LHB.bin");
#endif
    return rc;
}

int32_t FexTransfer::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mInitalized) {
        LOGE(mModule, "Thid moudle has not initalized\n");
        rc = NOT_INITED;
    } else {
        mInitalized = false;
    }



    return rc;
}

int32_t FexTransfer::fexUploadFile(const std::string &filePath)
{
    int32_t rc = NO_ERROR;
    CURL *curl = NULL;
    CURLcode res = CURLE_OK;
    struct curl_slist *headers = NULL;
    struct curl_httppost *post = NULL;
    struct curl_httppost *last = NULL;
    char *buffer = NULL;
    uint32_t length = 0;
    FILE *fp = fopen("/dev/null", "w");

    if (!mLoginStatus) {
        rc = NOT_READY;
    }

    if (SUCCEED(rc)) {
        curl = curl_easy_init();
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_URL, "http://fex.lenovo.com");
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COKIES_FILE); // 指定cookie文件
    }

    if (SUCCEED(rc)) {
        headers = curl_slist_append(headers, "Cache-Control: max-age=0");
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
        headers = curl_slist_append(headers, "Host: fex.lenovo.com");
        headers = curl_slist_append(headers, "Origin: http://fex.lenovo.com");
        headers = curl_slist_append(headers, "Referer: http://fex.lenovo.com/");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        if (NOTNULL(fp)) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        }
    }

    if (SUCCEED(rc)) {
        std::ifstream inStream(filePath, std::ios::binary | std::ios::ate);
        if (!inStream.is_open()) {
            LOGE(mModule, "File %s not exit\n", filePath.c_str());
            rc = NOT_FOUND;
        }

        if (SUCCEED(rc)) {
            length = inStream.tellg();
            if (length == 0) {
                LOGE(mModule, "file is empty\n");
                rc = NOT_EXIST;
            }
        }

        if (SUCCEED(rc)) {
            buffer = new char[length];
            if (ISNULL(buffer)) {
                LOGE(mModule, "Out of memory");
                rc = NO_MEMORY;
            } else {
                memset(buffer, 0, length);
            }
        }

        if (SUCCEED(rc)) {
            inStream.seekg(0);
            inStream.read(buffer, length);
        }

    }

    if (SUCCEED(rc)) {
        auto const pos = filePath.find_last_of('/');
        auto const pathTemp = filePath.substr(pos + 1);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "uploadfile",
                     CURLFORM_BUFFER, pathTemp.c_str(),
                     CURLFORM_BUFFERPTR, buffer,
                     CURLFORM_BUFFERLENGTH, length,
                     CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
    }

    if (SUCCEED(rc)) {
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (NOTNULL(curl)) {
        curl_easy_cleanup(curl);
    }

    if (NOTNULL(headers)) {
        curl_slist_free_all(headers);
    }

    SECURE_DELETE(buffer);

    if (NOTNULL(fp)) {
        fclose(fp);
    }
    return rc;
}

int32_t FexTransfer::fexDownloadFile(std::string& filelist, std::string storageFiles)
{
    int32_t rc = NO_ERROR;
    CURL *curl = NULL;
    CURLcode res = CURLE_OK;
    FILE *fp = NULL;


    if (!mLoginStatus) {
        LOGE(mModule, "Not ready!");
        rc = NOT_READY;
    }

    if (SUCCEED(rc)) {
        fp = fopen(storageFiles.c_str(), "w");
        if (ISNULL(fp)) {
            LOGE(mModule, "fopen %s failed", storageFiles.c_str());
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        curl = curl_easy_init();
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
#if 0
        std::string httpPath = "http://fex.lenovo.com/uploads/";
        httpPath += mName.c_str();
        httpPath += "/";
        httpPath += filelist;
#else
        std::string httpPath = "http://fex.lenovo.com/downfile/0";
        LOGE(mModule, "LHB %s", httpPath.c_str());
#endif
        curl_easy_setopt(curl, CURLOPT_URL, httpPath.c_str());
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COKIES_FILE); // 指定cookie文件
        // curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wirte_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    }

    if (SUCCEED(rc)) {
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (NOTNULL(fp)) {
        fclose(fp);
        fp = NULL;
    }

    if (NOTNULL(curl)) {
        curl_easy_cleanup(curl);
    }
    return rc;
}

int32_t FexTransfer::fexFileList(std::string& filePath)
{
    int32_t rc = NO_ERROR;
    CURL *curl = NULL;
    CURLcode res = CURLE_OK;
    char *retBuf = NULL;

    if (!mLoginStatus) {
        LOGE(mModule, "fex server not logined");
        rc = NOT_READY;
    }

    if (SUCCEED(rc)) {
        curl = curl_easy_init();
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        retBuf = new char[2048];
        if (ISNULL(retBuf)) {
            LOGE(mModule, "Out of memory");
            rc  = NO_MEMORY;
        } else {
            memset(retBuf, 0, 2048);
        }
    }

    if (SUCCEED(rc)) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://fex.lenovo.com/filelist/1");
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COKIES_FILE); // 指定cookie文件
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wirte_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, retBuf);
    }

    if (SUCCEED(rc)) {
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    // LOGD(mModule, "LHB:retBuf = %s\n", retBuf);

    if (SUCCEED(rc)) {
        std::string tmpStr = retBuf;
        std::string::size_type position;
        std::string fileName;
        std::string date;
        position = tmpStr.find("date");
        if (position == tmpStr.npos) {
            LOGE(mModule, "Not found date");
            position = tmpStr.find("login?next");
            rc = NOT_FOUND;
            if (position != tmpStr.npos) {
                LOGE(mModule, "Find 'login?next' OK ");
                rc = NOT_READY;
            }
        } else {
            date = tmpStr.substr(position + 9);
            std::string::size_type pos1 = date.find('.');
            //std::string::size_type pos2 = date.find("-");
            if (pos1 != date.npos) {
                date.erase(pos1);
                // std::cout<<"LHB"<<date.substr(pos1)<<std::endl;
                // date = date.substr(pos2+1,(pos1-pos2-1));
                date.erase(std::find(date.begin(), date.end(), '-'));
                date.erase(std::find(date.begin(), date.end(), '-'));
                date.erase(std::find(date.begin(), date.end(), ' '));
                date.erase(std::find(date.begin(), date.end(), ':'));
                date.erase(std::find(date.begin(), date.end(), ':'));
                // date.erase(std::find(date.begin(),date.end(),'.'));

            }
            filePath = date;
            filePath += '_';
        }

        if (SUCCEED(rc)) {
            position = tmpStr.find("filename");
            if (position == tmpStr.npos) {
                LOGE(mModule, "Not found filename");
                rc = NOT_FOUND;
            } else {
                fileName = tmpStr.substr(position + 11);
                std::string::size_type pos1 = fileName.find("\"");
                if (pos1 != fileName.npos) {
                    fileName.erase(pos1);
                    // std::cout<<"LHB放大放大"<<tmpStr.substr(position+11,pos1-position)<<std::endl;
                    // fileName = tmpStr.substr(position+11,pos1-position -3);

                }
                filePath += fileName;
            }
        }
    }

    SECURE_DELETE(retBuf);

    if (NOTNULL(curl)) {
        curl_easy_cleanup(curl);
    }

    return rc;
}

bool FexTransfer::fexCheckLoginStatus()
{
    int32_t rc = NO_ERROR;
    CURL *curl = NULL;
    CURLcode res = CURLE_OK;
    char *retBuf = NULL;
    bool retBl = false;

    if (SUCCEED(rc)) {
        curl = curl_easy_init();
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        retBuf = new char[2048];
        if (ISNULL(retBuf)) {
            rc  = NO_MEMORY;
        } else {
            memset(retBuf, 0, 2048);
        }
    }

    if (SUCCEED(rc)) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://fex.lenovo.com/islogin");
        curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COKIES_FILE); // 指定cookie文件
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, wirte_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, retBuf);
    }

    if (SUCCEED(rc)) {
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (NOTNULL(strstr((char*)retBuf, "true"))) {
            retBl = true;
            LOGD(mModule, "check login status is TRUE");
            mLoginStatus = true;
        } else {
            LOGD(mModule, "check login status is FALSE");
            retBl = false;
            mLoginStatus = false;
        }
    }

    SECURE_DELETE(retBuf);

    if (NOTNULL(curl)) {
        curl_easy_cleanup(curl);
    }

    return retBl;
}

int32_t FexTransfer::fexLogin()
{
    int32_t rc = NO_ERROR;
    CURL *curl = NULL;
    CURLcode res = CURLE_OK;
    struct curl_slist *headers = NULL;
    struct curl_httppost *post = NULL;
    struct curl_httppost *last = NULL;
    FILE *fp = fopen("post.html", "w");

    if (SUCCEED(rc)) {
        curl = curl_easy_init();
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        headers = curl_slist_append(headers, "Host:fex.lenovo.com");
        headers = curl_slist_append(headers, "Referer:http://fex.lenovo.com/login");
        headers = curl_slist_append(headers, "User-Agent:Mozilla/5.0");
        headers = curl_slist_append(headers, "Accept:*/*");
        headers = curl_slist_append(headers, "Cache-Control:no-cache");
        //headers = curl_slist_append(headers,"accept-encoding:gzip, deflate");
        headers = curl_slist_append(headers, "Connection:keep-alive");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);// 改协议头
        curl_easy_setopt(curl, CURLOPT_URL, "http://fex.lenovo.com/login");
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    }

    if (SUCCEED(rc)) {
        res = curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COKIES_FILE); // 指定cookie文件
        if (res != CURLE_OK) {
            LOGE(mModule, "read cookies failed\n");
            curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); /* start cookie engine */
        }
    }

    if (SUCCEED(rc)) {
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "username",
                     CURLFORM_COPYCONTENTS, mName.c_str(),
                     CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "password",
                     CURLFORM_COPYCONTENTS, mPassWd.c_str(),
                     CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "next",
                     CURLFORM_COPYCONTENTS, "/",
                     CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
    }

    if (SUCCEED(rc)) {
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            LOGE(mModule, "Curl perform failed: %s\n", curl_easy_strerror(res));
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        // export cookies
        curl_easy_setopt(curl, CURLOPT_COOKIEJAR, COKIES_FILE);
    }

    if (NOTNULL(curl)) {
        curl_easy_cleanup(curl);
    }
#if 0
    if (NOTNULL(post)) {
        curl_formfree(post);
    }

    if (NOTNULL(last)) {
        curl_formfree(last);
    }
#endif
    if (NOTNULL(headers)) {
        curl_slist_free_all(headers);
    }
    return rc;
}

int32_t FexTransfer::folder_mkdirs(const char *folder_path)
{
    int i, len;
    char str[512];
    strncpy(str, folder_path, 512);
    len = strlen(str);
    for (i = 0; i < len; i++) {
        if (str[i] == '/') {
            str[i] = '\0';
            if (access(str, 0) != 0) {
                mkdir(str, 0777);
            }
            str[i] = '/';
        }
    }
    if (len > 0 && access(str, 0) != 0) {
        mkdir(str, 0777);
    }

    return NO_ERROR;
}

uint32_t FexTransfer::pushData(TRANSFER_BUFFER_T &cmd)
{
    int32_t rc = 0;
    std::string filePath = WORK_DIRPATH;

    if (mTranDirct == TRAN_CLINET) {
        filePath += CLINET_PATH;
    } else {
        filePath += SERVER_PATH;
    }


    if (SUCCEED(rc)) {
        if (cmd.mode != TRAN_MODE_FEX) {
            rc = -1;
            LOGE(mModule, "Mode mismatching!");
        }
    }

    if (SUCCEED(rc)) {
        folder_mkdirs(filePath.c_str());
    }

    if (SUCCEED(rc)) {
#if 0
        cmdStr = "cp -ar ";
        cmdStr += (char *)cmd.buffer;
        cmdStr += " ";
#endif
        switch (mTranDirct) {
            case TRAN_CLINET:
                filePath += CLINETFILE;
                break;
            case TRANS_SERVER:
                filePath += SERVERFILE;
                break;
            default:
                rc = -1;
                LOGE(mModule, "Not support!");
                break;
        }
        // LOGD(mModule, "LHB filePath =%s\n", filePath.c_str());
        rc = rename((char*)cmd.buffer, filePath.c_str());
        if (FAILED(rc)) {
            LOGE(mModule, "Renaming file Error");
        }
    }
#if 0
    if (SUCCEED(rc)) {
        cmdStr += filePath;
        /* copy orig file to fex used files */
        rc = system((const char *) cmdStr.c_str());
    }
#endif

    if (SUCCEED(rc)) {
#if 0
        cmdStr = ("fex -u ");
        cmdStr += filePath;
        pthread_mutex_lock(&mTransMutex);
        rc = system((const char *) cmdStr.c_str());
        pthread_mutex_unlock(&mTransMutex);
#else

        pthread_mutex_lock(&mTransMutex);
        rc = fexUploadFile(filePath);
        pthread_mutex_unlock(&mTransMutex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed to fex upload files");
        }
#endif
    }

    return rc;
}

uint32_t FexTransfer::pullData(TRANSFER_BUFFER_T &cmd)
{
    int32_t rc = 0;
    std::string filePath = WORK_DIRPATH;
    std::string fileList;
    std::string receiveData;
    if (mTranDirct == TRAN_CLINET) {
        filePath += CLINET_PATH;
    } else {
        filePath += SERVER_PATH;
    }

    if (SUCCEED(rc)) {
        folder_mkdirs(filePath.c_str());
    }

    if (SUCCEED(rc)) {
        pthread_mutex_lock(&mTransMutex);
        rc = fexFileList(fileList);
        pthread_mutex_unlock(&mTransMutex);
        if (FAILED(rc)) {
            if (rc == NOT_READY) {
                LOGD(mModule, "Need Reloging ========================");
                /* --TODO-- need to login again*/
                pthread_mutex_lock(&mTransMutex);
                fexLogin();
                fexCheckLoginStatus();
                pthread_mutex_unlock(&mTransMutex);
            }
            LOGE(mModule, "Get file list failed\n");
        }
    }

    if (SUCCEED(rc)) {
        if (0 == fileList.compare(mLastFilelist)) {
            rc  = NOT_READY;
        } else {
            mLastFilelist = fileList;
        }
    }

    if (SUCCEED(rc)) {
        std::string::size_type position;
        switch (mTranDirct) {
            case TRAN_CLINET:
                position = fileList.find(SERVERFILE);
                if (position == fileList.npos) {
                    // LOGD(mModule, "Not found data %s ", SERVERFILE);
                    rc = NOT_FOUND;
                }
                filePath += SERVERFILE;
                break;
            case TRANS_SERVER:
                position = fileList.find(CLINETFILE);
                if (position == fileList.npos) {
                    // LOGD(mModule, "Not found data %s ", CLINETFILE);
                    rc = NOT_FOUND;
                }
                filePath += CLINETFILE;
                break;
            default:
                rc = -1;
                LOGE(mModule, "Not support!");
                break;
        }
    }

    if (SUCCEED(rc)) {
        LOGD(mModule, "fileList = %s filePath=%s", fileList.c_str(), filePath.c_str());
        pthread_mutex_lock(&mTransMutex);
        rc = fexDownloadFile(fileList, filePath);
        pthread_mutex_unlock(&mTransMutex);
        if (FAILED(rc)) {
            LOGE(mModule, "Failed fexDownloadFiles");
        }
    }

    if (SUCCEED(rc)) {
        memset(cmd.buffer, 0, cmd.length);
        strcpy((char*) cmd.buffer, filePath.c_str());
    }
    return rc;
}

uint32_t FexTransfer::readFile(const std::string &filePath, uint8_t *pBuffer, uint32_t &length)
{
    int32_t rc = 0;
    FILE    *pFile = NULL;
    uint32_t retSize;

    /* check file is exited */
    if (SUCCEED(rc)) {
        rc = access(filePath.c_str(), F_OK);
        if (FAILED(rc)) {
            LOGE(mModule, "File %s not exit\n", filePath.c_str());
        }
    }

    if (SUCCEED(rc)) {
        /* open file use read  only mode */
        pFile = fopen(filePath.c_str(), "r");
        if (ISNULL(pFile)) {
            rc = -3;
            LOGE(mModule, "Open file failed\n");
        }
        /* get file length */
        fseek(pFile, 0, SEEK_END);
        retSize = ftell(pFile);
        rewind(pFile);
    }

    if (SUCCEED(rc)) {
        if (retSize > length) {
            rc = -1;
            LOGE(mModule, "buffer is smaler!\n");
        }
    }

    if (SUCCEED(rc)) {
        /* read datas */
        length = fread(pBuffer, 1, retSize, pFile);
        if (retSize != length) {
            rc = -1;
            LOGE(mModule, "read length smaller than file size\n");
        }

    }

    if (NOTNULL(pFile)) {
        fclose(pFile);
    }

    return rc;
}

uint32_t FexTransfer::writeFile(const std::string &filePath, const uint8_t *buffer, uint32_t length)
{
    int32_t rc = 0;

    FILE    *pFile = NULL;

    if (SUCCEED(rc)) {
        /* open file use read  only mode */
        pFile = fopen(filePath.c_str(), "w+");
        if (ISNULL(pFile)) {
            rc = -1;
            LOGE(mModule, "Open file failed\n");
        }
        fwrite(buffer, length, 1, pFile);
    }

    if (NOTNULL(pFile)) {
        fclose(pFile);
    }

    return rc;
}

time_t FexTransfer::str_to_time_t(const std::string& ATime, const std::string& AFormat)
{
    struct tm tm_Temp;
    time_t time_Ret;
    try {
        sscanf(ATime.c_str(), AFormat.c_str(),// "%d/%d/%d %d:%d:%d" ,
               & (tm_Temp.tm_year),
               &(tm_Temp.tm_mon),
               &(tm_Temp.tm_mday),
               &(tm_Temp.tm_hour),
               &(tm_Temp.tm_min),
               &(tm_Temp.tm_sec),
               &(tm_Temp.tm_wday),
               &(tm_Temp.tm_yday));

        tm_Temp.tm_isdst = 0;
        time_Ret = mktime(&tm_Temp);
        return time_Ret;
    } catch (...) {
        return 0;
    }
}

};
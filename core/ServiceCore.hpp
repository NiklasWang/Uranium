#ifndef __SERVICE_CORE_HPP_
#define __SERVICE_CORE_HPP_

namespace uranium
{

std::string ServiceCore::getUserName()
{
    uid_t userid;
    struct passwd* pwd;
    userid = getuid();
    pwd = getpwuid(userid);
    LOGD(mModule, "User name is: %s\n", pwd->pw_name);
    return pwd->pw_name;
}

TRAN_HEADE_T* ServiceCore::createTranHeade(void)
{
    TRAN_HEADE_T *pTranHead = new TRAN_HEADE_T;
    memset(pTranHead, 0, sizeof(TRAN_HEADE_T));
    pTranHead->flages = EVENT_FLAGE_MASK;
    return pTranHead;
}

int32_t ServiceCore::destoryTranHeade(TRAN_HEADE_T *tranHead)
{
    SECURE_DELETE(tranHead);
    return NO_ERROR;
}

int32_t ServiceCore::transferDictionaryCMD(TREANFER_EVENT_ENUM evt, uint32_t cmdValue, bool sendOn)
{
    int32_t rc = NO_ERROR;
    std::string storagePath = appendBasePath(DIR_FILE_NAME);

    if (SUCCEED(rc)) {
        /* --TODO-- Package  the request command */
        TRAN_HEADE_T *pTranHead = createTranHeade();
        if (NOTNULL(pTranHead)) {
            /* Fill load command */
            pTranHead->evtKey = evt;
            pTranHead->evtValue = cmdValue;
            std::ofstream ostream(storagePath, std::ios::binary | std::ios::trunc);
            ostream.write((char *)pTranHead, sizeof(TRAN_HEADE_T));
            ostream.close();
            destoryTranHeade(pTranHead);
            if (sendOn) {
                mTransCore->send(storagePath);
            }
        } else {
            LOGE(mModule, "Out of memory\n");
        }
    }

    return rc;
}

int32_t ServiceCore::transferAppendData(const std::string &inFilePath, const std::string &outFilePath)
{
    int32_t rc = NO_ERROR;
    uint32_t size = 0;
    /* --TODO-- examine file is exit */
    std::ofstream ostream(outFilePath, std::ios::binary | std::ios::app);
    std::ifstream inStream(inFilePath, std::ios::binary | std::ios::ate);

    if (!inStream.is_open()) {
        LOGE(mModule, "File %s not exit\n", inFilePath.c_str());
        rc = NOT_FOUND;
    }

    if (!ostream.is_open()) {
        LOGE(mModule, "File %s not exit\n", inFilePath.c_str());
        rc = NOT_FOUND;
    }

    if (SUCCEED(rc)) {
        size = inStream.tellg();
        if (size == 0) {
            LOGE(mModule, "file is empty\n");
            rc = NOT_EXIST;
        }
    }

    if (SUCCEED(rc)) {
        inStream.seekg(0);
        char *buffer = new char[WRITE_BUFFER_PAGE];
        if (ISNULL(buffer)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Out of memory\n");
        } else {
            uint32_t page = size / WRITE_BUFFER_PAGE;
            uint32_t last_size = size % WRITE_BUFFER_PAGE;
            LOGD(mModule, "Page = %d,  last_size = %d", page, last_size);
            for (uint32_t i = 0; i < page; i++) {
                memset(buffer, 0, WRITE_BUFFER_PAGE);
                inStream.read(buffer, WRITE_BUFFER_PAGE);
                ostream.write(buffer, WRITE_BUFFER_PAGE);
            }
            if (last_size) {
                memset(buffer, 0, WRITE_BUFFER_PAGE);
                inStream.read(buffer, last_size);
                ostream.write(buffer, last_size);
            }
        }
        SECURE_DELETE(buffer);
    }


    inStream.close();
    ostream.close();

    if (SUCCEED(rc)) {
        mTransCore->send(outFilePath);
    }

    return rc;
}

int32_t ServiceCore::transferAppendData(const std::string &filePath)
{
    std::string storagePath =  appendBasePath(DIR_FILE_NAME);
    return transferAppendData(filePath, storagePath);
}

int32_t ServiceCore::reduceTranHeaderData(const std::string &inPath, const std::string &outPath)
{
    int32_t rc = NO_ERROR;
    std::ofstream ouStream(outPath, std::ios::binary | std::ios::trunc);
    std::ifstream inStream(inPath, std::ios::binary | std::ios::ate);
    char *buffer = NULL;
    uint32_t size = 0;

    if (!ouStream.is_open()) {
        LOGE(mModule, "Open %s failed\n", outPath.c_str());
        rc = NOT_FOUND;
    }

    if (!inStream.is_open()) {
        LOGE(mModule, "Open %s failed\n", inPath.c_str());
        rc = NOT_FOUND;
    }

    if (SUCCEED(rc)) {
        size = inStream.tellg();
    }

    if (SUCCEED(rc)) {
        inStream.seekg(sizeof(TRAN_HEADE_T));
        size -= sizeof(TRAN_HEADE_T);
    }

    if (SUCCEED(rc)) {
        buffer = new char[WRITE_BUFFER_PAGE];
        if (ISNULL(buffer)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Out of memory\n");
        }
    }

    if (SUCCEED(rc)) {
        uint32_t page = size / WRITE_BUFFER_PAGE;
        uint32_t last_size = size % WRITE_BUFFER_PAGE;
        for (uint32_t i = 0; i < page; i++) {
            memset(buffer, 0, WRITE_BUFFER_PAGE);
            inStream.read(buffer, WRITE_BUFFER_PAGE);
            ouStream.write(buffer, WRITE_BUFFER_PAGE);
        }
        if (last_size) {
            memset(buffer, 0, WRITE_BUFFER_PAGE);
            inStream.read(buffer, last_size);
            ouStream.write(buffer, last_size);
        }

    }

    SECURE_DELETE(buffer);
    inStream.close();
    ouStream.close();

    return rc;
}

int32_t ServiceCore::createEntryFile(const std::string &fileName, uint32_t value, bool fistFlage)
{
    std::string storageFile = appendBasePath(TRA_SYNC_FILE_NAME);
    std::string origFile = mLocalPath + fileName;
    std::ofstream ouStream;
    int32_t rc = NO_ERROR;
    TRANSFER_ENTRY_FILE_T entry;
    std::ifstream inStream(origFile, std::ios::binary | std::ios::ate);

    if (fistFlage) {
        ouStream.open(storageFile, std::ios::binary | std::ios::trunc);
    } else {
        ouStream.open(storageFile, std::ios::binary | std::ios::app);
    }

    if (!ouStream.is_open()) {
        LOGE(mModule, "Open %s failed\n", storageFile.c_str());
        rc = NOT_FOUND;
    }

    if (SUCCEED(rc)) {
        memset(&entry, 0, sizeof(entry));
        entry.flages = MOENTRY_FLAGE_MASK;
        strcpy(entry.fileName, fileName.c_str());
        entry.value = value;

        switch (value) {
            case MONITOR_Removed:
                entry.fileSize = 0;
                LOGD(mModule, "remove %s %d", fileName.c_str(), value);
                ouStream.write((char *)&entry, sizeof(TRANSFER_ENTRY_FILE_T));
                break;
            case MONITOR_Updated:
            case MONITOR_Created: {
                int32_t rc = NO_ERROR;

                if (SUCCEED(rc)) {
                    if (!inStream.is_open()) {
                        LOGE(mModule, "Open %s failed\n", origFile.c_str());
                        rc = NOT_FOUND;
                    }
                }
                if (SUCCEED(rc)) {
                    LOGD(mModule, "update %s %d", fileName.c_str(), value);
                    entry.fileSize = inStream.tellg();
                    inStream.seekg(0);
                }

                if (SUCCEED(rc)) {
                    char *buffer = new char[WRITE_BUFFER_PAGE];
                    if (ISNULL(buffer)) {
                        rc = NO_MEMORY;
                        LOGE(mModule, "Out of memory\n");
                    } else {
                        uint32_t page = entry.fileSize / WRITE_BUFFER_PAGE;
                        uint32_t last_size = entry.fileSize % WRITE_BUFFER_PAGE;
                        ouStream.write((char *)&entry, sizeof(TRANSFER_ENTRY_FILE_T));
                        for (uint32_t i = 0; i < page; i++) {
                            memset(buffer, 0, WRITE_BUFFER_PAGE);
                            inStream.read(buffer, WRITE_BUFFER_PAGE);
                            ouStream.write(buffer, WRITE_BUFFER_PAGE);
                        }
                        if (last_size) {
                            memset(buffer, 0, WRITE_BUFFER_PAGE);
                            inStream.read(buffer, last_size);
                            ouStream.write(buffer, last_size);
                        }
                    }
                    SECURE_DELETE(buffer);
                }

            }
            break;
            default:
                LOGE(mModule, "DANGER*** Do not suppot\n");
                break;
        }
    }

    {
        ouStream.close();
        inStream.close();
    }

    return rc;
}

int32_t ServiceCore::praseEntryFile(const std::string& inPath)
{
    std::ifstream inStream(inPath, std::ios::binary);
    uint32_t offset = 0;
    int32_t rc = NO_ERROR;

    if (!inStream.is_open()) {
        LOGE(mModule, "Open faile %s failed\n", inPath.c_str());
        rc = NOT_FOUND;
    }

    if (SUCCEED(rc)) {
        inStream.seekg(0);
        TRANSFER_ENTRY_FILE_T entry;
        while (!inStream.eof()) {
            memset(&entry, 0, sizeof(entry));
            inStream.read((char *)&entry, sizeof(TRANSFER_ENTRY_FILE_T));
            LOGD(mModule, "Flages = 0x%x 0x%x", entry.flages, MOENTRY_FLAGE_MASK);
            if (entry.flages != MOENTRY_FLAGE_MASK) {
                std::cout << "Flage not match error\n";
                break;
            }

            /* Intercept relative path */
            std::string tmpFilePath;
            auto commStrDirs = mLocalPath;
            commStrDirs[commStrDirs.length() - 1] = 0;
            auto posend = commStrDirs.find_last_of("/");
            if (posend == commStrDirs.npos) {
                LOGE(mModule, "%s find '/' in %s failed", __func__, commStrDirs.c_str());
                rc = UNKNOWN_ERROR;
            }

            auto strSubPath = commStrDirs.substr(0, posend + 1);
            commStrDirs = mLocalPath.substr(strSubPath.length());
            LOGD(mModule, "Dirname = %s", commStrDirs.c_str());
            std::string encryFileName = entry.fileName;
            posend = encryFileName.find(commStrDirs);
            if (posend != 0) {
                LOGE(mModule, "Dir not matching origFilePath=%s",  entry.fileName);
                LOGE(mModule, "Dir not matching origFilePath=%s",  entry.fileName);
                LOGE(mModule, "Dir not matching origFilePath=%s",  entry.fileName);
                rc = UNKNOWN_ERROR;
            }

            if (SUCCEED(rc)) {
                tmpFilePath = strSubPath;
                tmpFilePath += encryFileName;
                LOGD(mModule, "Update file Path = %s", tmpFilePath.c_str());
            }

            LOGD(mModule, "SYNC file path = %s", tmpFilePath.c_str());

            switch (entry.value) {
                case MONITOR_Removed: {
                    /* --TODO-- need to change */
                    LOGD(mModule, "MONITOR_Removed runing...");
                    std::string cmdStr = "rm -rf ";
                    cmdStr += tmpFilePath;
                    system(cmdStr.c_str());
                }
                break;

                case MONITOR_Updated:
                case MONITOR_Created: {
                    LOGD(mModule, "MONITOR_Created/MONITOR_Updated runing...");
                    int32_t rc = NO_ERROR;

                    char *buffer = new char[WRITE_BUFFER_PAGE];
                    if (ISNULL(buffer)) {
                        rc = NO_MEMORY;
                        LOGE(mModule, "memory is not ernough\n");
                    }

                    if (SUCCEED(rc)) {
                        std::ofstream ouStream(tmpFilePath, std::ios::binary | std::ios::trunc);

                        if (!ouStream.is_open()) {
                            LOGE(mModule, "Open %s failed\n", tmpFilePath.c_str());
                            rc = NOT_FOUND;
                        }
                        if (SUCCEED(rc)) {
                            uint32_t page = entry.fileSize / WRITE_BUFFER_PAGE;
                            uint32_t last_size = entry.fileSize % WRITE_BUFFER_PAGE;
                            for (uint32_t i = 0; i < page; i++) {
                                memset(buffer, 0, WRITE_BUFFER_PAGE);
                                inStream.read((char *)buffer, WRITE_BUFFER_PAGE);
                                ouStream.write(buffer, WRITE_BUFFER_PAGE);
                            }
                            if (last_size) {
                                memset(buffer, 0, WRITE_BUFFER_PAGE);
                                inStream.read((char *)buffer, last_size);
                                ouStream.write(buffer, last_size);
                            }

                        }

                        ouStream.close();
                    }

                    SECURE_DELETE(buffer);
                }
                break;
                default:
                    break;
            }
            offset += (entry.fileSize + sizeof(TRANSFER_ENTRY_FILE_T));
            inStream.seekg(offset);
        }
    }

    inStream.close();
    return NO_ERROR;
}

std::string ServiceCore::appendBasePath(const std::string dirPath)
{
    std::string storagePath = WORK_DIRPATH;

    if (TRAN_CLINET == mTranStatus) {
        storagePath += CLINET_PATH;
    } else {
        storagePath += SERVER_PATH;
    }
    storagePath += dirPath;
    return storagePath;
}

bool ServiceCore::isEmpty(const std::string dirPath)
{
    bool rc = true;
    DIR *dir = opendir(dirPath.c_str());
    struct dirent *ent;
    if (dir == NULL) {
        LOGE(mModule, "seekkey.c-98-opendir\n");
        rc = false;
    }
    if (rc) {
        while (1) {
            ent = readdir(dir);
            if (ent <= 0) {
                break;
            }
            if ((strcmp(".", ent->d_name) == 0) || (strcmp("..", ent->d_name) == 0)) {
                continue;
            }
            if ((ent->d_type == 4) || (ent->d_type == 8)) {
                rc = false;
                break;
            }
        }
    }

    if (NOTNULL(dir)) {
        closedir(dir);
    }

    return rc;
}

uint32_t ServiceCore::folder_mkdirs(const char *folder_path)
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


}


#endif

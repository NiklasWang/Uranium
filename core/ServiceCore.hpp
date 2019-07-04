#ifndef __SERVICE_CORE_HPP_
#define __SERVICE_CORE_HPP_

namespace uranium
{

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
    std::ofstream ostream(outFilePath, std::ios::binary | std::ios::app);
    std::ifstream inStream(inFilePath, std::ios::binary | std::ios::ate);
    auto size = inStream.tellg();

    inStream.seekg(0);
    char *buffer = new char[size];

    if (ISNULL(buffer)) {
        rc = NO_MEMORY;
        LOGE(mModule, "Out of memory\n");
    } else {
        inStream.read(buffer, size);
        ostream.write(buffer, size);
        delete buffer;
    }

    inStream.close();
    ostream.close();
    mTransCore->send(outFilePath);
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

    auto size = inStream.tellg();

    if (SUCCEED(rc)) {
        inStream.seekg(sizeof(TRAN_HEADE_T));
        size -= sizeof(TRAN_HEADE_T);

        buffer = new char[size];
        if (ISNULL(buffer)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Out of memory\n");
        }
    }

    if (SUCCEED(rc)) {
        inStream.read(buffer, size);
        ouStream.write(buffer, size);
        delete buffer;
    }

    inStream.close();
    ouStream.close();

    return rc;
}
int32_t ServiceCore::createEntryFile(const std::string &fileName, uint32_t value, bool fistFlage)
{
    std::string storageFile = appendBasePath(TRA_SYNC_FILE_NAME);
    std::string origFile = mLocalPath + fileName;
    std::cout << "LHB my origFilePath = " << origFile << "value = 0x" << std::hex << value << std::endl;
    std::ofstream ouStream;
    if (fistFlage) {
        ouStream.open(storageFile, std::ios::binary | std::ios::trunc);
    } else {
        ouStream.open(storageFile, std::ios::binary | std::ios::app);
    }

    std::ifstream inStream(origFile, std::ios::binary | std::ios::ate);
    TRANSFER_ENTRY_FILE_T entry;

    memset(&entry, 0, sizeof(entry));
    entry.flages = MOENTRY_FLAGE_MASK;
    strcpy(entry.fileName, fileName.c_str());
    //
    entry.value = value;
    //

    switch (value) {
        case MONITOR_Removed:
            entry.fileSize = 0;
            std::cout << "LHB " << fileName << " value = " << value << std::endl;
            ouStream.write((char *)&entry, sizeof(TRANSFER_ENTRY_FILE_T));
            break;
        case MONITOR_Updated:
        case MONITOR_Created: {
            int32_t rc = NO_ERROR;
            if (SUCCEED(rc)) {
                std::cout << "LHB " << fileName << " value = " << value << std::endl;
                entry.fileSize = inStream.tellg();
                inStream.seekg(0);
            }

            char *buffer = new char[entry.fileSize];
            if (ISNULL(buffer)) {
                rc = NO_MEMORY;
                std::cout << "Out of memory\n";
            }

            if (SUCCEED(rc)) {
                ouStream.write((char *)&entry, sizeof(TRANSFER_ENTRY_FILE_T));
                inStream.read(buffer, entry.fileSize);
                ouStream.write(buffer, entry.fileSize);
            }

            delete buffer;
        }
        break;
        default:
            LOGE(mModule, "Do not suppot\n");
            break;
    }
    ouStream.close();
    inStream.close();
    return NO_ERROR;
}

int32_t ServiceCore::praseEntryFile(const std::string& inPath)
{
    std::ifstream inStream(inPath, std::ios::binary);
    uint32_t offset = 0;
    inStream.seekg(0);
    TRANSFER_ENTRY_FILE_T entry;
    while (!inStream.eof()) {
        memset(&entry, 0, sizeof(entry));
        inStream.read((char *)&entry, sizeof(TRANSFER_ENTRY_FILE_T));
        LOGD(mModule, "LHB Flages = 0x%x 0x%x", entry.flages, MOENTRY_FLAGE_MASK);
        if (entry.flages != MOENTRY_FLAGE_MASK) {
            std::cout << "Flage not match error\n";
            break;
        }

        switch (entry.value) {
            case MONITOR_Removed: {
                LOGD(mModule, "MONITOR_Removed runing...");
                std::string cmdStr = "rm -rf ";
                cmdStr += mLocalPath;
                cmdStr +=  entry.fileName;
                system(cmdStr.c_str());
            }
            break;

            case MONITOR_Updated:
            case MONITOR_Created: {
                LOGD(mModule, "MONITOR_Created/MONITOR_Updated runing...");
                int32_t rc = NO_ERROR;
                char *buffer = new char[entry.fileSize];
                if (ISNULL(buffer)) {
                    rc = NO_MEMORY;
                    LOGE(mModule, "memory is not ernough\n");
                }

                if (SUCCEED(rc)) {
                    std::string tmpFilePath = mLocalPath;
                    tmpFilePath += entry.fileName;
                    LOGD(mModule, "SYNC file path = %s", tmpFilePath.c_str());
                    inStream.read((char *)buffer, entry.fileSize);
                    std::ofstream ouStream(tmpFilePath, std::ios::binary | std::ios::trunc);
                    ouStream.write(buffer, entry.fileSize);
                    ouStream.close();
                }
                delete buffer;
            }
            break;

            default:
                break;
        }
        offset += (entry.fileSize + sizeof(TRANSFER_ENTRY_FILE_T));
        inStream.seekg(offset);
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

}


#endif
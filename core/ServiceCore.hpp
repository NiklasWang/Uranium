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
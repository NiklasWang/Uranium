#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "logs.h"
#include "MemMgmt.h"
#include "EncryptFile.h"
#include "md5.h"
#include "sha.h"
#include "aes.h"

namespace uranium
{

using namespace std;

#define CHECK_ERROR(cond, retval, lable, fmt, args...)  \
do {                                                            \
    if(cond) {                                                  \
        LOGE(mModule, fmt, ##args);                             \
        LOGE(mModule, "%s() return %d\n",                       \
                __func__, retval);                              \
        rc = retval;                                            \
        goto lable;                                             \
    }                                                           \
} while(0)


EncryptFile::EncryptFile():
    mModule(MODULE_ENCRYPT)
{

}

EncryptFile::~EncryptFile()
{

}

uint8_t* EncryptFile::readFile(const std::string &filePath, uint32_t &length)
{
    int32_t rc = 0;
    FILE    *pFile = NULL;
    uint32_t retSize;
    uint8_t *pBuffer = NULL;

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
        /* get file lenght */
        fseek(pFile, 0, SEEK_END);
        length = ftell(pFile);
        rewind(pFile);
    }

    if (SUCCEED(rc)) {
        pBuffer = (uint8_t *)new char[length];
        if (ISNULL(pBuffer)) {
            rc = -1;
            LOGE(mModule, "malloc buffer failed\n");
        }
    }

    if (SUCCEED(rc)) {
        /* read datas */
        retSize = fread(pBuffer, 1, length, pFile);
        if (retSize != length) {
            rc = -1;
            LOGE(mModule, "read length smaller than file size\n");
        }

    }

    if (NOTNULL(pFile)) {
        fclose(pFile);
    }

    if (FAILED(rc)) {
        SECURE_DELETE(pBuffer);
        return NULL;
    }

    return pBuffer;
}

uint32_t EncryptFile::readBufferDestory(uint8_t *buffer)
{
    int32_t rc = 0;
    SECURE_DELETE(buffer);
    return rc;
}

uint32_t EncryptFile::writeFile(const std::string &filePath, const uint8_t *buffer, uint32_t length)
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

uint32_t EncryptFile::writeFileAppend(const std::string &filePath, const uint8_t *buffer, uint32_t length)
{
    int32_t rc = 0;
    FILE    *pFile = NULL;

    if (SUCCEED(rc)) {
        /* check file is exited */
        rc = access(filePath.c_str(), F_OK);
        if (FAILED(rc)) {
            LOGE(mModule, "File %s not exit\n", filePath.c_str());
        }
    }

    if (SUCCEED(rc)) {
        /* open file use read  only mode */
        pFile = fopen(filePath.c_str(), "a+");
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

int EncryptFile::encryptStream(const string& origFile, const string& destFile, const unsigned char* key16)
{
    uint32_t origFileLength = 0;
    uint32_t destFileLength = 0;
    uint8_t *pOrigBuffer = NULL;
    uint8_t *pDestBuffer = NULL;
    FILE_HEADER_T *pFileHeader = NULL;
    uint8_t ivs[16];
    AES_KEY aesKs;
    int rc = 0;

    if (SUCCEED(rc)) {
        pOrigBuffer = readFile(origFile, origFileLength);
        if (ISNULL(pOrigBuffer)) {
            rc = -1;
            LOGE(mModule, "read file failed!\n");
        }
    }
    if (SUCCEED(rc))  {
        pFileHeader = new FILE_HEADER_T;
        if (ISNULL(pFileHeader)) {
            rc = -1;
            LOGE(mModule, "out of memory\n");
        }
    }
    if (SUCCEED(rc)) {
        memset(pFileHeader, 0, sizeof(FILE_HEADER_T));
        pFileHeader->magicID = MAGIC_ID;
        pFileHeader->fileLength = origFileLength;
        md5_buffer((const char*)pOrigBuffer, origFileLength, pFileHeader->checksum);
    }

    if (SUCCEED(rc)) {
        destFileLength = align_len_to_size(origFileLength, 16);
        pDestBuffer = (uint8_t*) new uint8_t[destFileLength];
        if (ISNULL(pDestBuffer)) {
            rc = -1;
            LOGE(mModule, "out of memory\n");
        }
    }

    if (SUCCEED(rc)) {
        memset(ivs, 0, sizeof(ivs));
        private_AES_set_encrypt_key(key16, 128, &aesKs);
        AES_cbc_encrypt(pOrigBuffer, pDestBuffer, origFileLength, &aesKs,  ivs, 1);
    }

    if (SUCCEED(rc)) {
        writeFile(destFile, (const uint8_t *)pFileHeader, sizeof(FILE_HEADER_T));
        writeFileAppend(destFile, pDestBuffer, destFileLength);
    }

    readBufferDestory(pOrigBuffer);
    SECURE_DELETE(pDestBuffer);
    SECURE_DELETE(pFileHeader);
    return rc;
}

int EncryptFile::decryptStream(const string& origFile, const string& destFile, const unsigned char* key16)
{
    uint32_t origFileLength = 0;
    uint32_t destFileLength = 0;
    uint8_t *pOrigBuffer = NULL;
    uint8_t *pDestBuffer = NULL;
    uint8_t *pStartFile = NULL;
    FILE_HEADER_T *pFileHeader = NULL;
    uint8_t ivs[16];
    AES_KEY aesKs;
    int rc = 0;

    if (SUCCEED(rc)) {
        pOrigBuffer = readFile(origFile, origFileLength);
        if (ISNULL(pOrigBuffer)) {
            rc = -1;
            LOGE(mModule, "read file failed!\n");
        }
    }

    if (SUCCEED(rc))  {
        pFileHeader = new FILE_HEADER_T;
        if (ISNULL(pFileHeader)) {
            rc = -1;
            LOGE(mModule, "out of memory\n");
        }
    }

    if (SUCCEED(rc)) {
        memcpy(pFileHeader, pOrigBuffer, sizeof(FILE_HEADER_T));
        if (pFileHeader->magicID != MAGIC_ID) {
            rc = -1;
            LOGE(mModule, "files magicID failed!\n");
        }
    }

    if (SUCCEED(rc)) {
        destFileLength = align_len_to_size(pFileHeader->fileLength, 16);
        pStartFile = pOrigBuffer + sizeof(FILE_HEADER_T);
        pDestBuffer = new uint8_t[destFileLength];
        if (ISNULL(pDestBuffer)) {
            rc = -1;
            LOGE(mModule, "Out of memory\n");
        }
    }

    if (SUCCEED(rc)) {
        memset(pDestBuffer, 0, destFileLength);
        private_AES_set_decrypt_key(key16, 128, &aesKs);
        memset(ivs, 0, sizeof(ivs));
        AES_cbc_encrypt(pStartFile, pDestBuffer, destFileLength, &aesKs, (unsigned char*) ivs, 0);

        /* storage orig checksum */
        memcpy(mOrigChecsum, pFileHeader->checksum, sizeof(mOrigChecsum));

        /* calculate checksum */
        memset(mCalculateChecksum, 0, sizeof(mCalculateChecksum));
        md5_buffer((const char*)pDestBuffer, pFileHeader->fileLength, mCalculateChecksum);
    }

    if (SUCCEED(rc)) {
        writeFile(destFile, pDestBuffer, pFileHeader->fileLength);
    }

    readBufferDestory(pOrigBuffer);
    SECURE_DELETE(pDestBuffer);
    SECURE_DELETE(pFileHeader);

    return rc;
}

int EncryptFile::decryptStream(const std::string& origFile, const std::string& destFile,
                               unsigned int (&origChecksum)[4], unsigned int (&calculateChecksum)[4])
{
    int rc = 0;
    rc = decryptStream(origFile, destFile, NULL);
    if (FAILED(rc)) {
        return rc;
    }

    memcpy(origChecksum, mOrigChecsum, sizeof(mOrigChecsum));
    memcpy(calculateChecksum, mCalculateChecksum, sizeof(mCalculateChecksum));
    return rc;
}

int EncryptFile::decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16,
                               unsigned int (&origChecksum)[4], unsigned int (&calculateChecksum)[4])
{
    int rc = 0;
    rc = decryptStream(origFile, destFile, key16);
    if (FAILED(rc)) {
        return rc;
    }

    memcpy(origChecksum, mOrigChecsum, sizeof(mOrigChecsum));
    memcpy(calculateChecksum, mCalculateChecksum, sizeof(mCalculateChecksum));
    return rc;
}

};

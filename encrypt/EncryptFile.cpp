#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <functional>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <fstream>

#include "common.h"
#include "common.h"
#include "logs.h"
#include "MemMgmt.h"
#include "EncryptFile.h"
#include "md5.h"
#include "sha.h"
#include "aes.h"
#include "ServerConfig.h"

namespace uranium
{

using namespace std;


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
    char *pOrigBuffer = NULL;
    char *pDestBuffer = NULL;
    FILE_HEADER_T *pFileHeader = NULL;
    uint8_t ivs[16];
    AES_KEY aesKs;
    std::ofstream ostream(destFile, std::ios::binary | std::ios::trunc);
    std::ifstream inStream(origFile, std::ios::binary | std::ios::ate);
    int32_t rc = 0;

    if(!inStream.is_open() || !ostream.is_open()) {
        LOGE(mModule, "File %s or %s  not exit", origFile.c_str(),  destFile.c_str());
        rc |= NOT_FOUND;
    }

    if (SUCCEED(rc))  {
        pFileHeader = new FILE_HEADER_T;
        if (ISNULL(pFileHeader)) {
            rc = -1;
            LOGE(mModule, "out of memory\n");
        }
    }

    /* read file length */
    if(SUCCEED(rc)) {
        origFileLength = inStream.tellg();
        if (origFileLength == 0) {
            LOGE(mModule, "file is empty\n");
            rc = NOT_EXIST;
        }
        inStream.seekg(0);     
    }

    /* calcule md5sum */
    if (SUCCEED(rc)) {
        memset(pFileHeader, 0, sizeof(FILE_HEADER_T));
        pFileHeader->magicID = MAGIC_ID;
        pFileHeader->fileLength = origFileLength;
        FILE *pFile = fopen(origFile.c_str(), "r");
        if(ISNULL(pFile)) {
            LOGE(mModule, "Open file %s failed\n", origFile.c_str());
            rc = NOT_FOUND;
        }else {
           md5_stream(pFile, pFileHeader->checksum); 
           fclose(pFile);
           ostream.write((char *)pFileHeader, sizeof(FILE_HEADER_T)); 
        }
    }
    
    if (SUCCEED(rc)) {
        memset(ivs, 0, sizeof(ivs));
        private_AES_set_encrypt_key(key16, 128, &aesKs);
    }

    if(SUCCEED(rc)) {
        pOrigBuffer = new char[WRITE_BUFFER_PAGE];
        pDestBuffer = new char[WRITE_BUFFER_PAGE];
        if(ISNULL(pOrigBuffer) || ISNULL(pDestBuffer)) {
            LOGE(mModule, "Out of memory\n");
            rc = NO_MEMORY;
        }
    }

    if(SUCCEED(rc)) {
        uint32_t page = origFileLength / WRITE_BUFFER_PAGE;
        uint32_t last_size = origFileLength % WRITE_BUFFER_PAGE;
        for(uint32_t i = 0; i< page; i++)
        {   
            memset(pOrigBuffer, 0, WRITE_BUFFER_PAGE);
            memset(pDestBuffer, 0, WRITE_BUFFER_PAGE);
            inStream.read(pOrigBuffer, WRITE_BUFFER_PAGE);
            AES_cbc_encrypt((uint8_t *) pOrigBuffer, (uint8_t *)pDestBuffer, WRITE_BUFFER_PAGE, &aesKs,  ivs, 1);
            ostream.write(pDestBuffer, WRITE_BUFFER_PAGE); 
        }
        if(last_size) {
            memset(pOrigBuffer, 0, WRITE_BUFFER_PAGE);
            memset(pDestBuffer, 0, WRITE_BUFFER_PAGE);
            inStream.read(pOrigBuffer, last_size);
            last_size = align_len_to_size(last_size, 16);
            AES_cbc_encrypt((uint8_t *) pOrigBuffer,(uint8_t *) pDestBuffer, last_size, &aesKs,  ivs, 1);
            ostream.write(pDestBuffer, last_size);       
        }
    }    

    SECURE_DELETE(pOrigBuffer);
    SECURE_DELETE(pDestBuffer);
    SECURE_DELETE(pFileHeader);
    inStream.close();
    ostream.close();

    return rc;
}

int EncryptFile::decryptStream(const string& origFile, const string& destFile, const unsigned char* key16)
{
    char *oriBuffer = NULL;
    char *dstBuffer = NULL;
    FILE_HEADER_T *pFileHeader = new FILE_HEADER_T;
    std::ofstream ostream(destFile, std::ios::binary | std::ios::trunc);
    std::ifstream inStream(origFile, std::ios::binary | std::ios::ate);
    uint8_t ivs[16];
    AES_KEY aesKs;
    int rc = 0;

    if(!ostream.is_open() || !inStream.is_open()) {
        LOGE(mModule, "File %s or %s  not exit", origFile.c_str(),  destFile.c_str());
        rc = NOT_FOUND;
    }

    if(SUCCEED(rc)) {
        if(ISNULL(pFileHeader)) {
            LOGE(mModule, "Out of memory");
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        uint32_t size = inStream.tellg();
        inStream.seekg(0); 
        if(size < sizeof(FILE_HEADER_T)) {
            LOGE(mModule, "origFile size is error\n");
            rc = NO_MEMORY;
        }else{
            inStream.read((char *)pFileHeader, sizeof(FILE_HEADER_T));
            if(pFileHeader->magicID != MAGIC_ID) {
                LOGE(mModule, "encry file magicID failed\n");
                rc = NOT_SUPPORTED;
            }

            if(size < pFileHeader->fileLength) {
                LOGE(mModule,"file lose data\n");
                rc = BAD_PROTOCAL;
            }
        }
    }

    if(SUCCEED(rc)) {
        private_AES_set_decrypt_key(key16, 128, &aesKs);
        memset(ivs, 0, sizeof(ivs));        
    }

    if(SUCCEED(rc)) {
        oriBuffer = new char[WRITE_BUFFER_PAGE];
        dstBuffer = new char[WRITE_BUFFER_PAGE];
        if(ISNULL(oriBuffer) || ISNULL(dstBuffer)) {
            rc = NO_MEMORY;
            LOGE(mModule, "Out of memory");
        }
    }

    if(SUCCEED(rc)) {
        uint32_t page = pFileHeader->fileLength / WRITE_BUFFER_PAGE;
        uint32_t last_size = pFileHeader->fileLength % WRITE_BUFFER_PAGE;
        uint32_t last_size16 = align_len_to_size(last_size, 16);
        for(uint32_t i =0; i<page; i++) 
        {
            memset(oriBuffer, 0 , WRITE_BUFFER_PAGE);
            memset(dstBuffer, 0 , WRITE_BUFFER_PAGE);
            inStream.read(oriBuffer, WRITE_BUFFER_PAGE); 
            AES_cbc_encrypt((uint8_t *)oriBuffer,(uint8_t *) dstBuffer, WRITE_BUFFER_PAGE, &aesKs, (unsigned char*) ivs, 0);
            ostream.write(dstBuffer, WRITE_BUFFER_PAGE);
        }
        if(last_size) {
            memset(oriBuffer, 0 , WRITE_BUFFER_PAGE);
            memset(dstBuffer, 0 , WRITE_BUFFER_PAGE);
            inStream.read(oriBuffer, last_size16); 
            AES_cbc_encrypt((uint8_t *) oriBuffer,(uint8_t *) dstBuffer, last_size16, &aesKs, (unsigned char*) ivs, 0);
            ostream.write(dstBuffer, last_size);            
        }
    }

    if (SUCCEED(rc)) {
        /* storage orig checksum */
        memcpy(mOrigChecsum, pFileHeader->checksum, sizeof(mOrigChecsum));
        /* calculate checksum */
        memset(mCalculateChecksum, 0, sizeof(mCalculateChecksum));
        FILE *pFile = fopen(destFile.c_str(), "r");
        if(ISNULL(pFile)) {
            LOGE(mModule, "Open file %s failed\n", destFile.c_str());
            rc = NOT_FOUND;
        }else {
            md5_stream(pFile, mCalculateChecksum);
            fclose(pFile);
        }

    }

    SECURE_DELETE(oriBuffer);
    SECURE_DELETE(dstBuffer);
    SECURE_DELETE(pFileHeader);
    inStream.close();
    ostream.close();

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

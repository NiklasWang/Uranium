/**
 * @file CryptoAes.cpp
 * @brief  encrypt files
 * @author  lenovo <who@lenovo.com>
 * @version 1.0.0
 * @date 2019-05-29
 */

/* Copyright(C) 2009-2019, Lenovo Inc.
 * All right reserved
 *
 */

#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "common.h"
#include "logs.h"
#include "MemMgmt.h"
#include "CryptoAes.h"
#include "md5.h"
#include "sha.h"
#include "aes.h"


namespace uranium {

#define ROUND(r)        ((r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5))
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

/* --------------------------------------------------------------------------*/
/**
 * @brief CryptoAes
 *
 * @param ):
 */
/* --------------------------------------------------------------------------*/
CryptoAes::CryptoAes():
    mModule(MODULE_ENCRYPT),
    mEncTime(0)
{
    mEncTime = 0;
    mKeys = (unsigned char *) malloc(sizeof(unsigned char) * 1000 * 16);
    ASSERT_LOG(mModule, ISNULL(mKeys), "Malloc memory failed!\n");
    cryptoMakeKeys();
}

/* --------------------------------------------------------------------------*/
/**
 * @brief ~CryptoAes
 */
/* --------------------------------------------------------------------------*/
CryptoAes::~CryptoAes()
{
    SECURE_FREE(mKeys);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief randomRange
 *
 * @param a
 * @param b
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
double CryptoAes::randomRange(double a, double b)
{
    double x = (double) rand() / RAND_MAX;
    double result = x * (b - a) + a;
    return ROUND(result);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief cryptoMakeKeys
 */
/* --------------------------------------------------------------------------*/
void CryptoAes::cryptoMakeKeys(void)
{
    for (int i = 0; i < 1000 * 16; i++) {
        mKeys[i] = (unsigned char) randomRange(0, 0xFF);
    }
}

/* --------------------------------------------------------------------------*/
/**
 * @brief getCryptoKeys
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
const unsigned char* CryptoAes::getCryptoKeys(void)
{
    return mKeys;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief encryptStream
 *
 * @param origFile
 * @param destFile
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
int CryptoAes::encryptStream(const string& origFile, const string& destFile)
{
    unsigned char *keyTemp = NULL;

    /* Calculate the key by times */
    mEncTime = time(NULL);
    keyTemp = &mKeys[(mEncTime % 1000) * 16];

    return encryptStream(origFile, destFile, (const unsigned char *)keyTemp);
}

/* --------------------------------------------------------------------------*/
/**
 * @brief encryptStream
 *
 * @param origFile
 * @param destFile
 * @param key16
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
int CryptoAes::encryptStream(const string& origFile, const string& destFile, const unsigned char* key16)
{
    FILE *pOrigStream = NULL;
    FILE *pDestStream = NULL;
    size_t origFileLength = 0;
    size_t destFileLength = 0;
    char *pOrigBuffer = NULL;
    char *pDestBuffer = NULL;
    aes_data_head_t *pAesExternHeader = NULL;
    char *pFileStart = NULL;
    AES_KEY aesKs;
    int rc = 0;
    size_t return_size = 0;
    FILE_HEADER_T *pEncFileHeader = NULL;

    pOrigStream = fopen(origFile.c_str(), "r");
    CHECK_ERROR(ISNULL(pOrigStream), -1, err, "open file %s failed\n");

    /* get file lenght */
    fseek(pOrigStream, 0, SEEK_END);
    origFileLength = ftell(pOrigStream);
    rewind(pOrigStream);

    /* malloc a buffer to storage origFile data */
    pOrigBuffer = new char[origFileLength + sizeof(aes_data_head_t)];
    /* examle pOrigBuffer is NULL */
    CHECK_ERROR(ISNULL(pOrigBuffer), -2, err, "Out of memory");

    /* memset */
    memset(pOrigBuffer, 0, (size_t) sizeof(pOrigBuffer));

    pAesExternHeader = reinterpret_cast<aes_data_head_t *>(pOrigBuffer);
    pFileStart = pOrigBuffer +  sizeof(aes_data_head_t);
    pAesExternHeader->magicID = MAGIC_ID;
    /* storage file lenght to data header */
    pAesExternHeader->date_length = origFileLength;

    /* read file data */
    return_size = fread(pFileStart, 1, origFileLength, pOrigStream);
    CHECK_ERROR(!EQUALPTR(return_size, origFileLength), -3, err, "read file cout failed! origFileLenght=%ld readFileLenght=%ld \n",
                origFileLength, return_size);

    /* calculate checksum */
    md5_buffer(pFileStart, pAesExternHeader->date_length, pAesExternHeader->checksum);

#if 0
    for (i = 0; i < 4; i++) {
        printf("%08x", pAesExternHeader->checksum[i]);
    }
    printf("\n");
#endif

    /* let's 16-byte alignment */
    destFileLength = origFileLength + sizeof(aes_data_head_t);
    destFileLength = (destFileLength & 0xF) ? ((destFileLength + 0x10) & (~0xF)) : destFileLength;
    destFileLength += sizeof(FILE_HEADER_T);


    /* malloc buffer storage enc data */
    pDestBuffer = new char[destFileLength];
    CHECK_ERROR(ISNULL(pDestBuffer), -4, err, "out of memory\n");

    /* storage start encfile times */
    pEncFileHeader = (FILE_HEADER_T *) pDestBuffer;
    pEncFileHeader->start_time = mEncTime;
    cout << "enc time " << pEncFileHeader->start_time << endl;
    /* make keys */
    private_AES_set_encrypt_key(key16, 128, &aesKs);
    /* encry files */
    memset(iv, 0, sizeof(iv));
    AES_cbc_encrypt((const unsigned char*) pOrigBuffer, (unsigned char *)(pDestBuffer + sizeof(FILE_HEADER_T)), (origFileLength + sizeof(aes_data_head_t)), &aesKs, iv, 1);

    /* open or create new file to storage encry data */
    pDestStream = fopen(destFile.c_str(), "w+");
    CHECK_ERROR(ISNULL(pDestStream), -5, err, "open file %s failed!\n", destFile.c_str());

    /* write data to the opend file */
    fwrite(pDestBuffer, destFileLength, 1, pDestStream);
    cout << "encry file ok" << endl;

    rc = 0;
err:
    if (!ISNULL(pOrigStream)) {
        fclose(pOrigStream);
    }

    if (!ISNULL(pDestStream)) {
        fclose(pDestStream);
    }

    SECURE_DELETE(pOrigBuffer);

    SECURE_DELETE(pDestBuffer);
    mEncTime = 0;
    return rc;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief decryptStream
 *
 * @param origFile
 * @param destFile
 * @param key16
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
int CryptoAes::decryptStream(const string& origFile, const string& destFile, const unsigned char* key16)
{
    FILE *pOrigStream = NULL;
    FILE *pDestStream = NULL;
    size_t origFileLength = 0;
    size_t destFileLength = 0;
    char *pOrigBuffer = NULL;
    char *pDestBuffer = NULL;
    aes_data_head_t *pAesExternHeader = NULL;
    char *pFileStart = NULL;
    AES_KEY aesKs;
    int rc = 0;
    size_t return_size = 0;
    time_t endTime;
    const unsigned char *pkerTemp = key16;
    FILE_HEADER_T * pEncFileHeader = NULL;

    pOrigStream = fopen(origFile.c_str(), "r");
    CHECK_ERROR(ISNULL(pOrigStream), -1, err, "open file %s filed!\n", origFile.c_str());

    /* get file lenght */
    fseek(pOrigStream, 0, SEEK_END);
    origFileLength = ftell(pOrigStream);
    rewind(pOrigStream);

    /* malloc a buffer to storage origFile data */
    pOrigBuffer = new char[origFileLength];
    CHECK_ERROR(ISNULL(pOrigBuffer), -2, err, "out of memory\n");

    memset(pOrigBuffer, 0, (size_t) sizeof(pOrigBuffer));

    /* read file data */
    return_size = fread(pOrigBuffer, 1, origFileLength, pOrigStream);
    CHECK_ERROR(!EQUALPTR(return_size, origFileLength), -3, err, "read file cout failed! origFileLenght=%ld readFileLenght=%ld\n",
                origFileLength, return_size);
    /* check for timeout */
    pEncFileHeader = (FILE_HEADER_T *) pOrigBuffer;
    endTime = time(NULL);
    cout << "dec time " << pEncFileHeader->start_time << endl;
    if (!ISZERO(pEncFileHeader->start_time)) {
        CHECK_ERROR((pEncFileHeader->start_time > endTime) || (pEncFileHeader->start_time + MAX_DENC_TIMES) < endTime,
                    -6, err, "out of times\n");
        /* generate enc keys */
        pkerTemp = &mKeys[(pEncFileHeader->start_time % 1000) * 16];
    }

    if (ISNULL(pkerTemp)) {
        pkerTemp = &mKeys[(pEncFileHeader->start_time % 1000) * 16];
    }

    /* malloc buffer to storage enc data */
    pDestBuffer = new char[origFileLength];
    CHECK_ERROR(ISNULL(pDestBuffer), -4, err, "out of memory!\n");

    /* make keys */
    private_AES_set_decrypt_key(pkerTemp, 128, &aesKs);
    /* encry files */
    memset(iv, 0, sizeof(iv));
    AES_cbc_encrypt((const unsigned char *)(pOrigBuffer + sizeof(FILE_HEADER_T)), (unsigned char *) pDestBuffer,
                    origFileLength - sizeof(FILE_HEADER_T), &aesKs, iv, 0);

    pAesExternHeader = reinterpret_cast<aes_data_head_t *>(pDestBuffer);
    /* examale decry file data is success */
    CHECK_ERROR(!EQUALPTR(pAesExternHeader->magicID, MAGIC_ID), -7, err, "decry file failed!\n");

    destFileLength = pAesExternHeader->date_length;
    pFileStart = pDestBuffer + sizeof(aes_data_head_t);

    /* storage orig checksum */
    memcpy(mOrigChecsum, pAesExternHeader->checksum, sizeof(mOrigChecsum));

    /* calculate checksum */
    memset(mCalculateChecksum, 0, sizeof(mCalculateChecksum));
    md5_buffer(pFileStart, destFileLength, mCalculateChecksum);
    /* open or create new file to storage encry data */
    pDestStream = fopen(destFile.c_str(), "w+");
    CHECK_ERROR(ISNULL(pDestStream), -5, err, "open faile %s failed!\n", destFile.c_str());

    /* write data to the opend file */
    fwrite(pFileStart, destFileLength, 1, pDestStream);
    cout << "decry file ok" << endl;
    rc = 0;

err:
    if (!ISNULL(pOrigStream)) {
        fclose(pOrigStream);
    }
    if (!ISNULL(pDestStream)) {
        fclose(pDestStream);
    }

    SECURE_DELETE(pOrigBuffer);
    SECURE_DELETE(pDestBuffer);
    mEncTime  = 0;

    return rc;
}

/* --------------------------------------------------------------------------*/
/**
 * @brief decryptStream
 *
 * @param origFile
 * @param destFile
 * @param &origChecksum
 * @param &calculateChecksum
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
int CryptoAes::decryptStream(const std::string& origFile, const std::string& destFile,
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

/* --------------------------------------------------------------------------*/
/**
 * @brief decryptStream
 *
 * @param origFile
 * @param destFile
 * @param key16
 * @param &origChecksum
 * @param &calculateChecksum
 *
 * @return
 */
/* --------------------------------------------------------------------------*/
int CryptoAes::decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16,
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

}

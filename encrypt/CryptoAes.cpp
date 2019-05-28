
#include <iostream>
#include <string>
#include "common.h"
#include "logs.h"
#include "CryptoAes.h"
#include "md5.h"
#include "sha.h"
extern "C" {
#include <stdio.h>
#include <string.h>
#include "aes.h"
}

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

CryptoAes::CryptoAes():
    mModule(MODULE_ENCRYPT)
{

}

CryptoAes::~CryptoAes()
{

}

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

    /* malloc buffer storage enc data */
    pDestBuffer = new char[destFileLength];
    CHECK_ERROR(ISNULL(pDestBuffer), -4, err, "out of memory\n");

    /* make keys */
    private_AES_set_encrypt_key(key16, 128, &aesKs);
    /* encry files */
    memset(iv, 0, sizeof(iv));
    AES_cbc_encrypt((const unsigned char*) pOrigBuffer, (unsigned char *) pDestBuffer, (origFileLength + sizeof(aes_data_head_t)), &aesKs, iv, 1);

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

    return rc;
}

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

    /* malloc buffer storage enc data */
    pDestBuffer = new char[origFileLength];
    CHECK_ERROR(ISNULL(pDestBuffer), -4, err, "out of memory!\n");

    /* make keys */
    private_AES_set_decrypt_key(key16, 128, &aesKs);
    /* encry files */
    memset(iv, 0, sizeof(iv));
    AES_cbc_encrypt((const unsigned char *)pOrigBuffer, (unsigned char *) pDestBuffer, origFileLength, &aesKs, iv, 0);

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

    return rc;
}

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
    return 0;
}
} /* namespace sirius */

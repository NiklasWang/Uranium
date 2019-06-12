#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include "logs.h"
#include "common.h"
#include "MemMgmt.h"
#include "md5.h"
#include "sha.h"
#include "aes.h"
#include "uuid.h"
#include "Dictionary.h"

namespace uranium
{

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


static const uint8_t publicKeys[16] = {
    0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xBA,
    0x34, 0x56, 0x78, 0xBa, 0xbc, 0xde, 0xf7, 0x12
};

Dictionary::Dictionary():
    mModule(MODULE_ENCRYPT),
    fKeyInit(0)
{
    mKeys = (uint8_t *) malloc(sizeof(uint8_t) * MAX_KEYS * 16);
    ASSERT_LOG(mModule, ISNULL(mKeys), "Malloc memory failed!\n");
}

Dictionary::~Dictionary()
{
    if (!ISNULL(mKeys)) {
        free(mKeys);
    }
}

int32_t Dictionary::loadKeys(const std::string &path)
{
    int32_t rc = 0;
    FILE    *pFile = NULL;

    /* check file is exited */
    rc = access(path.c_str(), F_OK);
    CHECK_ERROR(FAILED(rc), -1, err, "File %s not exit\n", path.c_str());
    /* open file use read  only mode */
    pFile = fopen(path.c_str(), "r");
    CHECK_ERROR(ISNULL(pFile), -2, err, "Open file failed\n");

err:
    if (NOTNULL(pFile)) {
        fclose(pFile);
    }

    return rc;
}

int32_t Dictionary::storageKeys(const std::string &path)
{
    int32_t rc = 0;
    FILE    *pFile = NULL;

    /* check file is exited */
    rc = access(path.c_str(), F_OK);
    CHECK_ERROR(FAILED(rc), -1, err, "File %s not exit\n", path.c_str());
    /* open file use read  only mode */
    pFile = fopen(path.c_str(), "r");
    CHECK_ERROR(ISNULL(pFile), -2, err, "Open file failed\n");

    if (1 != fKeyInit) {
        makeKeys();
    }

    rc = 0;
err:
    if (NOTNULL(pFile)) {
        fclose(pFile);
    }

    return rc;
}

void Dictionary::makeKeys(void)
{

    for (int i = 0; i < MAX_KEYS; i++) {
        uuid_generate((mKeys + i * 16));
        printf("Key[i] = ");
        for (int j = 0; j < 16; j++) {
            printf("0x%02x ", mKeys[i * 16 + j]);
        }
        printf("\n");
    }
    fKeyInit = 1;
}

void Dictionary::setDictionary(const uint8_t *keys)
{
    uint32_t keySize = 0;
    keySize = sizeof(keys);
    if (keySize != MAX_KEYS * 16) {
        LOGE(mModule, "key size is not enough \n");
        return;
    }

    memcpy(mKeys, keys, (MAX_KEYS * 16));

}

const uint8_t* Dictionary::getDictionary(void)
{
    return mKeys;
}

template<class T, int N>
void Dictionary::getKeys(const struct timeval  &timeValue, T(&key)[N])
{
    uint32_t md5sum[4];
    uint32_t keyA;
    uint32_t keyB;
    uint32_t keyC;
    uint8_t buff[128];

    /* convet value to char string */
    memset(buff, 0, sizeof(buff));
    printf("time %ld%ld\n", (long int) timeValue.tv_sec, (long int) timeValue.tv_usec);
    sprintf(buff, "%ld%ld", timeValue.tv_sec, timeValue.tv_usec);
    printf("LHB %s lenght=%d\n", buff, strlen(buff));
    /* calcule timeValue md5sum */
    md5_buffer(buff, strlen(buff), md5sum);
    /* md5sum */
    keyA = md5sum[0] ^ md5sum[2];
    keyB = md5sum[1] ^ md5sum[3];
    keyC = ((keyA & 0xFFFF) << 16) + ((keyB >> 16) & 0xFFFF);
    /* total sum is MAX_KEYS*16 - last 16-bits let  max size is  (MAX_KEYS*16-17)*/
    keyC %= (((MAX_KEYS - 1) << 4) - 1);
    printf("N=%d\n", N);
    memcpy(key, &mKeys[keyC], N);
}

};



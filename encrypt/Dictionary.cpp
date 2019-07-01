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

#define DEBUG  0

namespace uranium
{


static const uint8_t publicKeys[16] = {
    0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xBA,
    0x34, 0x56, 0x78, 0xBa, 0xbc, 0xde, 0xf7, 0x12
};

void Dictionary::printfKeys(void)
{
    for (int i = 0; i < MAX_KEYS; i++) {
        printf("Key[%d] = ", i);
        for (int j = 0; j < 16; j++) {
            printf("0x%02x ", mKeys[i * 16 + j]);
        }
        printf("\n");
    }
}

void Dictionary::generateKeys(void)
{
    if (!fKeyInit) {
        for (int i = 0; i < MAX_KEYS; i++) {
            uuid_generate((mKeys + i * 16));
#if DEBUG
            printf("Key[%d] = ", i);
            for (int j = 0; j < 16; j++) {
                printf("0x%02x ", mKeys[i * 16 + j]);
            }
            printf("\n");
#endif
        }
        fKeyInit = true;
    }
}

int32_t Dictionary::loadDiction(const std::string  &filePath)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        std::FILE* f = std::fopen(filePath.c_str(), "r");
        if (!f) {
            rc = NO_MEMORY;
            LOGE(mModule, "Open %s failed\n", filePath.c_str());
        } else {
            auto size = std::fread(mKeys, 1, (sizeof(uint8_t) * MAX_KEYS * 16), f);
            std::cout << "LHB read size = " << size << "orig size =" << (sizeof(uint8_t) * MAX_KEYS * 16) << std::endl;
            std::fclose(f);
        }
    }

#if DEBUG
    printfKeys();
#endif
    return rc;
}

int32_t Dictionary::sotraDiction(const std::string &filePath)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        std::ofstream ouStream(filePath, std::ios::binary | std::ios::trunc);
        std::cout << "LHB bin size=" << sizeof(uint8_t) * MAX_KEYS * 16 << std::endl;
        ouStream.write((char *)mKeys, (sizeof(uint8_t) * MAX_KEYS * 16));
        ouStream.close();
    }

#if DEBUG
    printfKeys();
#endif
    return rc;
}
#if 0
template<class T, int N>
void Dictionary::getKeys(const struct timeval  &timeValue, T(&key)[N])
#endif
void Dictionary::getKeys(const struct timeval  &timeValue, char *key)
{
    uint32_t md5sum[4];
    uint32_t keyA;
    uint32_t keyB;
    uint32_t keyC;
    char buff[128];

    if (mDynamicKey) {
        memcpy(key, publicKeys, 16);
        return;
    } else {
        /* convet value to char string */
        memset(buff, 0, sizeof(buff));
        sprintf(buff, "%ld%ld", timeValue.tv_sec, timeValue.tv_usec);
        /* calcule timeValue md5sum */
        md5_buffer(buff, strlen(buff), md5sum);
        /* md5sum */
        keyA = md5sum[0] ^ md5sum[2];
        keyB = md5sum[1] ^ md5sum[3];
        keyC = ((keyA & 0xFFFF) << 16) + ((keyB >> 16) & 0xFFFF);
        /* total sum is MAX_KEYS*16 - last 16-bits let  max size is  (MAX_KEYS*16-17)*/
        keyC %= (((MAX_KEYS - 1) << 4) - 1);
        memcpy(key, &mKeys[keyC], 16);
    }
}

void Dictionary::setDynamicEnable(void)
{
    mDynamicKey = true;
}

void Dictionary::setDynamicDisable(void)
{
    mDynamicKey = false;
}

int32_t Dictionary::construct()
{
    int32_t rc = NO_ERROR;

    if (mConstructed) {
        rc = ALREADY_INITED;
    }

    if (SUCCEED(rc)) {
        mKeys = (uint8_t *) malloc(sizeof(uint8_t) * MAX_KEYS * 16);
        if (ISNULL(mKeys)) {
            LOGE(mModule, "Malloc memory failed!\n");
        }
    }

    return rc;
}

int32_t Dictionary::destruct()
{
    int32_t rc = NO_ERROR;

    if (!mConstructed) {
        rc = NOT_INITED;
    } else {
        mConstructed = false;
    }

    if (SUCCEED(rc)) {
        SECURE_FREE(mKeys);
    }
    return rc;
}

Dictionary::Dictionary():
    mConstructed(false),
    fKeyInit(false),
    mDynamicKey(false),
    mModule(MODULE_ENCRYPT),
    mKeys(NULL)
{


}

Dictionary::~Dictionary()
{
    if (mConstructed) {
        destruct();
    }
    fKeyInit = false;
    mConstructed = false;
}

}



/**
 * @file CryptoAes.h
 * @brief aes data extension header file
 * @author lenovo <lenovo.com>
 * @version 1.0.0
 * @date 2019-05-24
 */

/* Copyright(C) 2009-2017, Itarge Inc.
 * All right reserved
 *
 */

#ifndef __CRYPTOAES_H_
#define __CRYPTOAES_H_

#include <string>
#include "common.h"


namespace uranium
{

#define MAGIC_ID    (0xABBA)
typedef struct {
    unsigned int magicID;
    unsigned int checksum[4];
    unsigned int date_length;
    unsigned int reserve[3];
} aes_data_head_t;

class CryptoAes
{

public:
    CryptoAes();
    ~CryptoAes();

    int encryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);
    int decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);
    int decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16,
                      unsigned int (&origChecksum)[4], unsigned int (&calculateChecksum)[4]);
    /* int setKeys(const char& key16); */
private:
    unsigned char key16[16];
    unsigned char iv[16];
    unsigned int mOrigChecsum[4];
    unsigned int mCalculateChecksum[4];
    ModuleType      mModule;
};

int __EncryptTestMain(int argc, char **argv);
}; /* namespace sirius */

#endif //__CRYPTOAES_H_

/**
 * @file CryptoAes.h
 * @brief aes data extension header file
 * @author lenovo <lenovo.com>
 * @version 1.0.0
 * @date 2019-05-24
 */

/* Copyright(C) 2009-2019, Lenovo Inc.
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
#define MAX_DENC_TIMES  (2*60)

typedef struct FILE_HEADER_TAG {
    unsigned int magicID;
    time_t start_time;
} FILE_HEADER_T;

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
    int encryptMakeKeys(const std::string& filePath);
    /* --------------------------------------------------------------------------*/
    /**
     * @brief encryptStream
     *        encry files
     *        [ according timer to select default keys]
     *
     * @param[in] origFile  The path of while encrytp'd file
     * @param[in] destFile  The path of storate files
     *
     * @return
     *      0 -> success
     */
    /* --------------------------------------------------------------------------*/
    int encryptStream(const std::string& origFile, const std::string& destFile);

    /* --------------------------------------------------------------------------*/
    /**
     * @brief encryptStream
     *        encry files use user keys
     *
     * @param[in] origFile  The path of while encrytp'd file
     * @param[in] destFile  The path of storate files
     * @param[in] key16     16-bits user keys
     *
     * @return
     *      0 -> success
     *      other -> failed
     */
    /* --------------------------------------------------------------------------*/
    int encryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);

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
    int decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);

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
    int decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16,
                      unsigned int (&origChecksum)[4], unsigned int (&calculateChecksum)[4]);

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
    int decryptStream(const std::string& origFile, const std::string& destFile,
                      unsigned int (&origChecksum)[4], unsigned int (&calculateChecksum)[4]);

    /* --------------------------------------------------------------------------*/
    /**
     * @brief decryptKeys
     *
     * @return
     */
    /* --------------------------------------------------------------------------*/
    const unsigned char* getCryptoKeys(void);
private:
    unsigned char*   mKeys;
    unsigned char   iv[16];
    unsigned int    mOrigChecsum[4];
    unsigned int    mCalculateChecksum[4];
    ModuleType      mModule;
    time_t          mEncTime;

    CryptoAes(const CryptoAes &crpAes) = delete;
    CryptoAes &operator=(const CryptoAes &crpAes) = delete;

    double randomRange(double a, double b);
    void cryptoMakeKeys(void);
};

int __EncryptTestMain(int argc, char **argv);
}; /* namespace sirius */

#endif //__CRYPTOAES_H_

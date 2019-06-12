#ifndef __ENCRYPTFILE_H_
#define __ENCRYPTFILE_H_

#include <string>
#include "common.h"


namespace uranium
{

#define MAGIC_ID    (0xABBA)
#define MAX_DENC_TIMES  (2*60)

typedef struct {
    unsigned int magicID;
    unsigned int checksum[4];
    unsigned int fileLength;
} FILE_HEADER_T;

typedef struct {
    unsigned int magicID;
    unsigned int checksum[4];
    unsigned int date_length;
    unsigned int reserve[3];
} aes_data_head_t;

class EncryptFile
{

public:
    EncryptFile();
    ~EncryptFile();
    int encryptStream(const std::string& origFile, const std::string& destFile);
    int encryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);
    int decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);
    int decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16,
                      unsigned int (&origChecksum)[4], unsigned int (&calculateChecksum)[4]);
    int decryptStream(const std::string& origFile, const std::string& destFile,
                      unsigned int (&origChecksum)[4], unsigned int (&calculateChecksum)[4]);
    const unsigned char* getCryptoKeys(void);
private:
    unsigned char   iv[16];
    unsigned int    mOrigChecsum[4];
    unsigned int    mCalculateChecksum[4];
    ModuleType      mModule;

    uint8_t* readFile(const std::string &filePath, uint32_t &length);
    uint32_t readBufferDestory(uint8_t *buffer);
    uint32_t writeFile(const std::string &filePath, const uint8_t *buffer, uint32_t length);
    uint32_t writeFileAppend(const std::string &filePath, const uint8_t *buffer, uint32_t length);

    EncryptFile(const EncryptFile &crpAes) = delete;
    EncryptFile &operator=(const EncryptFile &crpAes) = delete;
};

int __EncryptTestMain(int argc, char **argv);
}; /* namespace sirius */

#endif //__ENCRYPTFILE_H_

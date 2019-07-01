#ifndef __ENCRYPT_CORE_H__
#define __ENCRYPT_CORE_H__

#include <functional>
#include <pthread.h>

#include "common.h"
#include "threads/ThreadPoolEx.h"
#include "Dictionary.h"
#include "EncryptFile.h"

namespace uranium
{
class Dictionary;
class EncryptFile;
class EncryptCore
{
public:
    int32_t generateDictionKeys(void);
    int32_t sotraDiction(const std::string  &filePath, std::function<int32_t ()>cb);
    int32_t loadDiction(const std::string &filePath);
    void setDynamicEnable(void);
    void setDynamicDisable(void);
    void getKeys(const struct timeval &timeValue, uint8_t *key);

public:
    int32_t encryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);
    int32_t decryptStream(const std::string& origFile, const std::string& destFile, const unsigned char* key16);

public:
    int32_t construct();
    int32_t destruct();
    EncryptCore();
    virtual ~EncryptCore();

private:
    EncryptCore(const EncryptCore &rhs) = delete;
    EncryptCore &operator=(const EncryptCore &rhs) = delete;

private:
    bool            mConstructed;
    bool            mSyncFlage;
    ModuleType      mModule;
    ThreadPoolEx    *mThreads;
    Dictionary      *mDiction;
    EncryptFile     *mEncryMan;
};

}

#endif

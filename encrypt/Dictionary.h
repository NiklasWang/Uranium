#ifndef __DICTIONARY__
#define __DICTIONARY__

#include <string>
#include <fstream>
#include "common.h"

namespace uranium
{
#define MAX_KEYS    (1000)

class Dictionary
{

public:
    void generateKeys(void);
    int32_t loadDiction(const std::string  &filePath);
    int32_t sotraDiction(const std::string &filePath);
#if 0
    template<class T, int N>
    void getKeys(const struct timeval &timeValue, T(&key)[N]);
#endif
    void getKeys(const struct timeval &timeValue, char *key);
    void setDynamicEnable(void);
    void setDynamicDisable(void);
#if 0
    void setDictionary(const uint8_t *keys);
    const uint8_t* getDictionary(void);
    template<class T, int N>
    void getKeys(const struct timeval &timeValue, T(&key)[N]);
    int32_t loadKeys(const std::string &path);
    int32_t storageKeys(const std::string &path);
#endif

public:
    int32_t construct();
    int32_t destruct();
    Dictionary();
    ~Dictionary();
private:
    void printfKeys();

private:
    Dictionary(const Dictionary &dire) = delete;
    Dictionary &operator=(const Dictionary &crpAes) = delete;

private:
    bool            mConstructed;
    bool            fKeyInit;
    bool            mDynamicKey;
    ModuleType      mModule;
    uint8_t         *mKeys;

};

}; /* namespace sirius */

#endif //__DICTIONARY__

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

#ifndef __DICTIONARY__
#define __DICTIONARY__

#include <string>
#include "common.h"

namespace uranium
{
#define MAX_KEYS    (1000)
class Dictionary
{

public:
    Dictionary();
    ~Dictionary();
    void makeKeys(void);
    void setDictionary(const uint8_t *keys);
    const uint8_t* getDictionary(void);
    template<class T, int N>
    void getKeys(const struct timeval &timeValue, T(&key)[N]);
    int32_t loadKeys(const std::string &path);
    int32_t storageKeys(const std::string &path);

private:
    uint8_t         *mKeys;
    ModuleType      mModule;
    uint8_t         fKeyInit;
    Dictionary(const Dictionary &dire) = delete;
    Dictionary &operator=(const Dictionary &crpAes) = delete;
};

}; /* namespace sirius */

#endif //__DICTIONARY__

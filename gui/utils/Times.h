#ifndef _TIME_EX_H_
#define _TIME_EX_H_

#include <sys/time.h>
#include <windows.h>
#include <stdint.h>

namespace uranium {

int64_t currentUs();

void delayMs(int32_t ms);

void delaySec(int32_t sec);

int32_t clock_gettime(int, struct timeval *tv);

struct Ms
{
public:
    Ms(int32_t ms);
    bool operator==(const Ms &);
    bool operator!=(const Ms &);
    Ms &operator=(const Ms &);
    int32_t operator()();

private:
    int32_t mMs;
};

struct Sec {
public:
    Sec(int32_t sec);
    bool operator==(const Sec &);
    bool operator!=(const Sec &);
    Sec &operator=(const Sec &);
    int32_t operator()();

private:
    int32_t mSec;
};

}

#endif

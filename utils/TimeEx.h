#ifndef _TIME_EX_H_
#define _TIME_EX_H_

#include <unistd.h>
#include <sys/time.h>

namespace sirius {

inline int64_t currentUs()
{
    int64_t t = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (int64_t)tv.tv_usec + tv.tv_sec * 1000000ll;

    return t;
}

inline void delayMs(int32_t ms)
{
    ms > 0 ? usleep(ms * 1000) : 0;
}

inline void delaySec(int32_t sec)
{
    delayMs(sec * 1000);
}

};

#endif

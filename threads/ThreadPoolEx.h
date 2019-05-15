#ifndef _THREAD_POOL_EX_H_
#define _THREAD_POOL_EX_H_

#include "ThreadPool.h"

namespace sirius {

class ThreadPoolEx :
    public ThreadPool {

public:
    static ThreadPoolEx *getInstance();
    static uint32_t removeInstance();

private:
    ThreadPoolEx(uint32_t c = 0);
    ThreadPoolEx(const ThreadPoolEx &) = delete;
    ThreadPoolEx &operator=(const ThreadPoolEx &) = delete;

public:
    virtual ~ThreadPoolEx();

private:
    static ModuleType gModule;
    static uint32_t   gCnt;
    static pthread_mutex_t mInsL;
    static ThreadPoolEx   *gThis;
};

};

#endif

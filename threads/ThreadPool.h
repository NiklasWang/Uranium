#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include "Thread.h"
#include "Semaphore.h"

namespace sirius {

class ThreadPool :
    virtual public noncopyable {

public:

    int32_t run(std::function<int32_t ()> func);

    template <typename T>
    int32_t run(std::function<int32_t (T *)> func, T *arg);

    int32_t runWait(std::function<int32_t ()> func);

    template <typename T>
    int32_t runWait(std::function<int32_t (T *)> func, T *arg);

    int32_t run(std::function<int32_t ()> func, sync_type sync);

    template <typename T>
    int32_t run(std::function<int32_t (T *)> func, T *arg, sync_type sync);

    bool available();
    int32_t workload();
    bool haveFree();
    bool withinCapacity();

private:
    typedef Thread WorkerThread;

    WorkerThread *get(BlockType type = BLOCK_TYPE);
    int32_t callback(WorkerThread *source);

public:
    explicit ThreadPool(uint32_t c = 0);
    virtual ~ThreadPool();

private:
    int32_t  mCnt;
    uint32_t mCapacity;
    bool     mDestruct;
    RWLock   mThreadLock;
    ModuleType mModule;
    QueueT<Semaphore> mSems;
    std::list<WorkerThread *> mThreads;
};

};

#include "ThreadPool.hpp"

#endif

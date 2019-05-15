#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include "ThreadIntf.h"
#include "WorkerThread.h"
#include "Semaphore.h"

namespace sirius {

class ThreadPool :
    public IStatusListener,
    public noncopyable {
public:
    bool available();
    sp<WorkerThread> get(BlockType type = BLOCK_TYPE);

private:
    bool haveFree();
    bool withinCapacity();
    virtual int32_t onStatusChanged(WorkerStatus status) override;

public:
    explicit ThreadPool(ThreadIntf *p, uint32_t c = 0);
    virtual ~ThreadPool();

private:
    ThreadPool(const ThreadPool &rhs) = delete;
    ThreadPool &operator=(const ThreadPool &rhs) = delete;

private:
    struct SemaphoreEx :
        public Semaphore,
        public RefBase {
    };

private:
    uint32_t mCnt;
    uint32_t mCapacity;
    RWLock   mWorkLock;
    RWLock   mSemLock;
    RWLock   mThreadLock;
    ModuleType mModule;
    List<sp<SemaphoreEx> > mSems;
    List<sp<WorkerThread> > mThreads;
    ThreadIntf *mParent;
};

};

#endif

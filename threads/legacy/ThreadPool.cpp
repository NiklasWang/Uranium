#include "ThreadPool.h"

namespace sirius {

ThreadPool::ThreadPool(ThreadIntf *p, uint32_t c) :
    mCnt(0), mCapacity(c),
    mModule(MODULE_THREAD_POOL), mParent(p)
{
}

ThreadPool::~ThreadPool()
{
    RWLock::AutoWLock l(mWorkLock);

    {
        RWLock::AutoWLock l(mSemLock);
        while(mSems.begin() != mSems.end()) {
            mSems.erase(mSems.begin());
        }
    }

    {
        RWLock::AutoWLock l(mThreadLock);
        for (auto &thread : mThreads) {
            if (!ISNULL(thread)) {
                thread->destruct();
            }
        }
    }
}

bool ThreadPool::available()
{
    RWLock::AutoRLock l(mWorkLock);
    return withinCapacity() || haveFree();
}

bool ThreadPool::haveFree()
{
    bool result = false;
    RWLock::AutoRLock l(mThreadLock);

    for (auto &thread : mThreads) {
        if (!thread->busy()) {
            result = true;
            break;
        }
    }

    return result;
}

bool ThreadPool::withinCapacity()
{
    return !mCapacity ? true : (mCnt < mCapacity);
}

sp<WorkerThread> ThreadPool::get(BlockType type)
{
    int32_t rc = NO_ERROR;
    bool create = true;
    sp<WorkerThread> result = NULL;
    RWLock::AutoRLock l(mWorkLock);

    do {
        {
            RWLock::AutoRLock l(mThreadLock);
            for (auto &thread : mThreads) {
                if (!thread->busy()) {
                    create = false;
                    result = thread;
                    break;
                }
            }
        }

        if (ISNULL(result) && (type == BLOCK_TYPE) && !withinCapacity()) {
            sp<SemaphoreEx> sem = new SemaphoreEx();
            {
                RWLock::AutoWLock l(mSemLock);
                mSems.push_back(sem);
            }
            sem->wait();
        } else {
            break;
        }
    } while (true);

    if (ISNULL(result) && (type == NONBLOCK_TYPE) && !withinCapacity()) {
        create = false;
    }

    if (create) {
        char name[255];
        sprintf(name, "WorkerThread##%d##", mCnt);
        sp<WorkerThread> thread = NULL;
        thread = new WorkerThread(mParent, name);
        if (ISNULL(thread)) {
            LOGE(mModule, "Failed to create worker thread");
            rc = UNKNOWN_ERROR;
        } else {
            rc = thread->construct();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to construct worker thread");
            } else {
                mCnt++;
                thread->registerListener(this);
                result = thread;
                RWLock::AutoWLock l(mThreadLock);
                mThreads.push_back(thread);
            }
        }
    }

    return result;
}

int32_t ThreadPool::onStatusChanged(WorkerStatus status)
{
    RWLock::AutoRLock l(mWorkLock);

    if (status == WORKER_STATUS_IDLE) {
        if ((mCapacity != 0) && (mCnt >= mCapacity)) {
            RWLock::AutoWLock l(mSemLock);
            for (auto &sem : mSems) {
                sem->signal();
                break;
            }
            if (mSems.begin() != mSems.end()) {
                mSems.erase(mSems.begin());
            }
        }
    }

    return NO_ERROR;
}

};


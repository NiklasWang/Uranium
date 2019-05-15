#include "ThreadPool.h"

namespace sirius {

int32_t ThreadPool::run(std::function<int32_t ()> func)
{
    return run(func, ASYNC_TYPE);
}

int32_t ThreadPool::runWait(std::function<int32_t ()> func)
{
    return run(func, SYNC_TYPE);
}

int32_t ThreadPool::run(std::function<int32_t ()> func, sync_type sync)
{
    int32_t rc = NO_ERROR;
    WorkerThread *worker = get(
        sync == SYNC_TYPE ? BLOCK_TYPE : NONBLOCK_TYPE);

    if (NOTNULL(worker)) {
        rc = worker->run(func, sync,
            [this](Thread *thread) -> int32_t {
                return callback(thread);
            }
        );
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to run on worker thread, %d", rc);
        }
    } else {
        LOGE(mModule, "Failed to find idle thread or create new one");
        rc = UNKNOWN_ERROR;
    }

    return rc;
}

#define CHECK_VALID_STATUS() \
    ({ \
        int32_t __rc = NO_ERROR; \
        if (mDestruct) { \
            __rc = NOT_INITED; \
            LOGE(mModule, "Thread pool destructed."); \
        } \
        __rc; \
    })


ThreadPool::ThreadPool(uint32_t c) :
    mCnt(0), mCapacity(c), mDestruct(false),
    mModule(MODULE_THREAD_POOL)
{
}

ThreadPool::~ThreadPool()
{
    mDestruct = true;

    Semaphore *sem = NULL;
    while(NOTNULL(sem = mSems.dequeue())) {
        sem->signal();
    }

    RWLock::AutoWLock l(mThreadLock);
    while(mThreads.begin() != mThreads.end()) {
        auto iter = mThreads.begin();
        WorkerThread *thread = *iter;
        if (NOTNULL(thread)) {
            thread->destruct();
            delete thread;
        }
        mThreads.erase(iter);
    }
}

bool ThreadPool::available()
{
    int32_t rc = CHECK_VALID_STATUS();
    bool result = false;

    if (SUCCEED(rc)) {
        result = withinCapacity() || haveFree();
    }

    return result;
}

int32_t ThreadPool::workload()
{
    return mCnt;
}

bool ThreadPool::haveFree()
{
    int32_t rc = CHECK_VALID_STATUS();
    bool result = false;

    if (SUCCEED(rc)) {
        RWLock::AutoRLock l(mThreadLock);
        for (auto &thread : mThreads) {
            if (thread->idle()) {
                result = true;
                break;
            }
        }
    }

    return result;
}

bool ThreadPool::withinCapacity()
{
    int32_t rc = CHECK_VALID_STATUS();
    bool result = false;

    if (SUCCEED(rc)) {
        int32_t cap = mCapacity;
        result = !mCapacity ? true : (mCnt < cap);
    }

    return result;
}

ThreadPool::WorkerThread *ThreadPool::get(BlockType type)
{
    int32_t rc = NO_ERROR;
    bool create = true;
    WorkerThread *result = NULL;

    do {
        {
            RWLock::AutoWLock l(mThreadLock);
            for (auto iter = mThreads.begin(); iter != mThreads.end(); iter++) {
                if ((*iter)->idle()) {
                    create = false;
                    result = *iter;
                    iter = mThreads.erase(iter);
                    break;
                }
            }
        }

        if (ISNULL(result) && (type == BLOCK_TYPE) && !withinCapacity()) {
            Semaphore *sem = new Semaphore();
            mSems.enqueue(sem);
            sem->wait();
            SECURE_DELETE(sem);
        } else {
            break;
        }

        if (mDestruct) {
            LOGE(mModule, "Failed to get thread when pool destructed.");
            break;
        }
    } while (true);

    if (ISNULL(result) && (type == NONBLOCK_TYPE) && !withinCapacity()) {
        create = false;
    }

    if (ISNULL(result) && mDestruct) {
        create = false;
    }

    if (create) {
        char name[255];
        utils_atomic_inc(&mCnt);
        sprintf(name, "WorkerThread##%d##", mCnt);
        WorkerThread *thread = NULL;
        thread = new WorkerThread(name);
        if (ISNULL(thread)) {
            LOGE(mModule, "Failed to create worker thread");
            utils_atomic_dec(&mCnt);
            rc = UNKNOWN_ERROR;
        } else {
            rc = thread->construct();
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to construct worker thread");
                utils_atomic_dec(&mCnt);
            } else {
                result = thread;
            }
        }
    }

    return result;
}

int32_t ThreadPool::callback(WorkerThread *source)
{
    int32_t rc = CHECK_VALID_STATUS();

    if (SUCCEED(rc)) {
        RWLock::AutoWLock l(mThreadLock);
        mThreads.push_back(source);
    }

    if (SUCCEED(rc)) {
        Semaphore *sem = mSems.dequeue();
        if (NOTNULL(sem)) {
            sem->signal();
        }
    }

    return rc;
}

};


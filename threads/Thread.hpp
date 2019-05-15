#include "MemMgmt.h"
#include "Thread.h"

namespace sirius {

#define INTERNAL true

template <typename T>
int32_t Thread::run(std::function<int32_t (T *)> func, T *arg,
    std::function<int32_t (Thread *)> cb)
{
    return run<T>(func, arg, ASYNC_TYPE, cb, INTERNAL);
}

template <typename T>
int32_t Thread::runWait(std::function<int32_t (T *)> func, T *arg,
    std::function<int32_t (Thread *)> cb)
{
    return run<T>(func, arg, SYNC_TYPE, cb, INTERNAL);
}

template <typename T>
int32_t Thread::run(std::function<int32_t (T *)> func, T *arg,
    sync_type sync, std::function<int32_t (Thread *)> cb)
{
    int32_t rc = NO_ERROR;

    if (sync == SYNC_TYPE) {
        rc = runWait<T>(func, arg, cb);
    }
    if (sync == ASYNC_TYPE) {
        rc = run<T>(func, arg, cb);
    }

    return rc;
}

template <typename T>
int32_t Thread::run(std::function<int32_t (T *)> func, T *arg,
    sync_type sync, std::function<int32_t (Thread *)> cb, bool /*internal*/)
{
    int32_t rc = NO_ERROR;
    bool buffered = true;
    TaskInf<T>  *task  = NULL;
    T           *t     = NULL;
    SyncType    *syncc = NULL;

    if (mStatus == THREAD_STATUS_UNINITED) {
        LOGE(mModule, "already exit");
        rc = NOT_INITED;
    } else {
        utils_atomic_inc(&mTaskCnt);
    }

    if (SUCCEED(rc)) {
        task = new TaskInf<T>();
        if (ISNULL(task)) {
            LOGE(mModule, "Failed to new buffered task info");
            rc = NO_MEMORY;
        } else {
            task->clear();
            task->set(THREAD_CONTROL_NEW_JOB);
            task->module = arg->getModule();
            task->cb = cb;
        }
    }

    if (SUCCEED(rc)) {
        if (sync == ASYNC_TYPE) {
            t = new T();
            if (ISNULL(t)) {
                LOGE(mModule, "Failed to new T arg");
                rc = NO_MEMORY;
            } else {
                *t = *arg;
            }
        }
        task->func = func;
        task->arg = sync == ASYNC_TYPE ? t : arg;
    }

    if (SUCCEED(rc)) {
        if (sync == SYNC_TYPE) {
            syncc = GET_OBJ_FROM_BUFFER(mSyncBuf);
            if (ISNULL(syncc)) {
                syncc = new SyncType();
                buffered = false;
            }
            if (ISNULL(syncc)) {
                LOGE(mModule, "Failed to alloc buffered sync control or new");
                rc = NO_MEMORY;
            } else {
                task->set(syncc, buffered);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (!mQ.enqueue(task)) {
            LOGE(mModule, "Failed to enqueue");
            rc = UNKNOWN_ERROR;
        } else {
            mSem.signal();
        }
    }

    if (SUCCEED(rc)) {
        if (sync == SYNC_TYPE) {
            syncc->wait();
            rc = task->rc;
            if (!SUCCEED(rc)) {
                LOGD(mModule, "Failed to do process task.");
            }
        }
    }

    if (SUCCEED(rc) || !SUCCEED(rc)) {
        if (sync == SYNC_TYPE) {
            SECURE_DELETE(task);
            if (!buffered && NOTNULL(syncc)) {
                SECURE_DELETE(syncc);
            }
            if (buffered && NOTNULL(syncc)) {
                PUT_OBJ_TO_BUFFER(mSyncBuf, syncc);
            }
        }
    }

    return rc;
}

template <typename T>
Thread::TaskInf<T>::TaskInf() :
    arg(NULL)
{}

template <typename T>
Thread::TaskInf<T>::TaskInf(const Thread::TaskInf<T> &rhs)
{
    if (this != &rhs) {
        *this = rhs;
    }
}

};


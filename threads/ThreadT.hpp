#include "MemMgmt.h"
#include "ThreadT.h"

namespace sirius {

#define INTERNAL true

template <typename T>
int32_t ThreadT<T>::run(std::function<int32_t ()> func)
{
    int32_t rc = NO_ERROR;

    return run(
        [func](int32_t * /*_rc*/) -> int32_t {
            return func();
        },
        &rc);
}

template <typename T>
int32_t ThreadT<T>::run(std::function<int32_t (T *)> func, T *arg)
{
    return run(func, arg, ASYNC_TYPE, INTERNAL);
}

template <typename T>
int32_t ThreadT<T>::runWait(std::function<int32_t ()> func)
{
    int32_t rc = NO_ERROR;

    return runWait(
        [func](int32_t * /*_rc*/) -> int32_t {
            return func();
        },
        &rc);
}

template <typename T>
int32_t ThreadT<T>::runWait(std::function<int32_t (T *)> func, T *arg)
{
    return run(func, arg, SYNC_TYPE, INTERNAL);
}

template <typename T>
int32_t ThreadT<T>::run(std::function<int32_t ()> func, sync_type sync)
{
    int32_t rc = NO_ERROR;

    if (sync == SYNC_TYPE) {
        rc = runWait(func);
    }
    if (sync == ASYNC_TYPE) {
        rc = run(func);
    }

    return rc;
}

template <typename T>
int32_t ThreadT<T>::run(std::function<int32_t (T *)> func, T *arg, sync_type sync)
{
    int32_t rc = NO_ERROR;

    if (sync == SYNC_TYPE) {
        rc = runWait(func, arg);
    }
    if (sync == ASYNC_TYPE) {
        rc = run(func, arg);
    }

    return rc;
}

#define THREAD_DELETE_PUT_BUFFERED(buffer, obj, buffered) \
    if (!buffered && NOTNULL(obj)) { \
        delete obj; \
    } \
    if (buffered && NOTNULL(obj)) { \
        PUT_OBJ_TO_BUFFER(buffer, obj); \
    }


template <typename T>
int32_t ThreadT<T>::run(std::function<int32_t (T *)> func,
    T *arg, sync_type sync, bool /*internal*/)
{
    int32_t rc = NO_ERROR;
    bool bufferedTask = true;
    bool bufferedArg  = true;
    bool bufferedSync = true;
    TaskInf  *task  = NULL;
    T        *t   = NULL;
    SyncType *syncc = NULL;

    if (mStatus == THREAD_STATUS_UNINITED) {
        LOGE(mModule, "already exit");
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        task = GET_OBJ_FROM_BUFFER(mTaskBuf);
        if (ISNULL(task)) {
            task = new TaskInf();
            bufferedTask = false;
        }
        if (ISNULL(task)) {
            LOGE(mModule, "Failed to alloc buffered task info or new");
            rc = NO_MEMORY;
        } else {
            task->clear();
            task->set(THREAD_CONTROL_NEW_JOB, bufferedTask);
        }
    }

    if (SUCCEED(rc)) {
        if (sync == ASYNC_TYPE) {
            t = GET_OBJ_FROM_BUFFER(mArgBuf);
            if (ISNULL(t)) {
                t = new T();
                bufferedArg = false;
            }
            if (ISNULL(t)) {
                LOGE(mModule, "Failed to alloc T arg or new");
                rc = NO_MEMORY;
            } else {
                *t = *arg;
                task->bufT = bufferedArg;
            }
        }
        if (SUCCEED(rc)) {
            if (sync == ASYNC_TYPE) {
                task->set(func, t, bufferedArg);
            }
            if (sync == SYNC_TYPE) {
                task->set(func, arg, false);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (sync == SYNC_TYPE) {
            syncc = GET_OBJ_FROM_BUFFER(mSyncBuf);
            if (ISNULL(syncc)) {
                syncc = new SyncType();
                bufferedSync = false;
            }
            if (ISNULL(syncc)) {
                LOGE(mModule, "Failed to alloc buffered sync control or new");
                rc = NO_MEMORY;
            } else {
                task->set(syncc, bufferedSync);
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
            THREAD_DELETE_PUT_BUFFERED(mSyncBuf, syncc, bufferedSync);
            THREAD_DELETE_PUT_BUFFERED(mTaskBuf, task,  bufferedTask);
        }
    }

    return rc;
}


template <typename T>
ThreadT<T>::ThreadT(std::string name) :
    mName(name),
    mTid(0),
    mStatus(THREAD_STATUS_UNINITED),
    mModule(MODULE_THREAD_POOL)
{
}

template <typename T>
ThreadT<T>::~ThreadT()
{
    if (mStatus != THREAD_STATUS_UNINITED) {
        destruct();
    }
}

template <typename T>
void *ThreadT<T>::runThread(void *thiz)
{
    assert(thiz != NULL);

    ThreadT *pme = static_cast<ThreadT *>(thiz);
    pme->threadLoop();

    return NULL;
}

template <typename T>
int32_t ThreadT<T>::threadLoop()
{
    int32_t rc = NO_ERROR;
    bool exit = false;
    TaskInf *task = NULL;

    LOGD(mModule, "E Thread %s created", whoamI());
    mReadySignal.signal();

    do {
        do {
            rc = mSem.wait();
            if (rc != 0 && errno != EINVAL) {
                LOGE(mModule, "thread cond wait error, %s", strerror(errno));
            }
        } while (rc != 0);

        mStatus = THREAD_STATUS_WORKING;
        task = mQ.dequeue();
        if (ISNULL(task)) {
            LOGE(mModule, "Dequeue failed");
            mStatus = THREAD_STATUS_RUNNING;
            continue;
        }

        switch (task->getType()) {
            case THREAD_CONTROL_NEW_JOB: {
                std::function<int32_t (T *)> func = task->getFunc();
                T *arg = task->getT();
                rc = func(arg);
                task->rc = rc;
                if (!SUCCEED(rc)) {
                    LOGD(mModule, "Task process failed. %d", rc);
                }
            } break;
            case THREAD_CONTROL_EXIT_THREAD: {
                exit = true;
            } break;
            default: {
                LOGE(mModule, "Oops, received unknown cmd %d, ignore",
                    task->getType());
            } break;
        }

        if (ISNULL(task->getSync())) { // ASYNC_TYPE
            if (task->getT()) {
                THREAD_DELETE_PUT_BUFFERED(mArgBuf, task->getT(), task->bufT);
            }
            THREAD_DELETE_PUT_BUFFERED(mTaskBuf, task, task->bufMe);
        } else {
            task->getSync()->signal();
        }

        mStatus = THREAD_STATUS_RUNNING;
    } while(!exit);

    mStatus = THREAD_STATUS_INITED;
    LOGD(mModule, "X Thread %s exited", whoamI());
    return rc;
}

template <typename T>
int32_t ThreadT<T>::construct()
{
    int32_t rc = NO_ERROR;

    if (mStatus != THREAD_STATUS_UNINITED) {
        LOGE(mModule, "thread exists");
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mStatus = THREAD_STATUS_INITED;

        if (pthread_create(&mTid, NULL, runThread, this)) {
            LOGE(mModule, "Failed to create working thread");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        pthread_setname_np(mTid, mName.c_str());
        mReadySignal.wait();
        mStatus = THREAD_STATUS_RUNNING;
    }

    if (!SUCCEED(rc) && rc != ALREADY_EXISTS) {
        LOGE(mModule, "Failed to start thread");
        mStatus = THREAD_STATUS_UNINITED;
        mTid = 0;
    }

    return rc;
}

template <typename T>
int32_t ThreadT<T>::destruct()
{
    int32_t rc = NO_ERROR;
    bool buffered = true;
    TaskInf *task  = NULL;

    if (mStatus == THREAD_STATUS_UNINITED) {
        LOGE(mModule, "already exit");
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        task = GET_OBJ_FROM_BUFFER(mTaskBuf);
        if (ISNULL(task)) {
            task = new TaskInf();
            buffered = false;
        }
        if (ISNULL(task)) {
            LOGE(mModule, "Failed to alloc buffered task info or new");
            rc = NO_MEMORY;
        } else {
            task->clear();
            // Sync mark set to ASYNC by clear()
            // This task object will put back in threadLoop
            // pthread_join will waitting for this operation
            task->set(THREAD_CONTROL_EXIT_THREAD, buffered);
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
        if (pthread_join(mTid, NULL)) {
            LOGE(mModule, "failed to stop thread");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        mStatus = THREAD_STATUS_UNINITED;
        mTid = 0;
    }

    if (SUCCEED(rc)) {
        TaskInf *iter = NULL;
        while(NOTNULL(iter = mQ.dequeue())) {
            if (iter->getSync()) {
                THREAD_DELETE_PUT_BUFFERED(mSyncBuf, iter->getSync(), iter->bufSync);
            }
            if (iter->getT()) {
                THREAD_DELETE_PUT_BUFFERED(mArgBuf, iter->getT(), iter->bufT);
            }
            THREAD_DELETE_PUT_BUFFERED(mTaskBuf, iter, iter->bufMe);
        }
        mQ.flush();
    }

    if (SUCCEED(rc)) {
        mTaskBuf.flush();
        mArgBuf.flush();
        mSyncBuf.flush();
    }

    return rc;
}

template <typename T>
const char *ThreadT<T>::whoamI()
{
    return mName.c_str();
}

template <typename T>
uint32_t ThreadT<T>::TaskInf::cnt = 0;

template <typename T>
ThreadT<T>::TaskInf::TaskInf() :
    bufMe(false),
    bufT(false),
    bufSync(false),
    rc(NO_ERROR),
    type(THREAD_CONTROL_EXIT_THREAD),
    sync(NULL),
    arg(NULL),
    index(cnt++)
{}

template <typename T>
ThreadT<T>::TaskInf::TaskInf(const ThreadT<T>::TaskInf &rhs)
{
    if (this != &rhs) {
        *this = rhs;
        index = cnt++;
    }
}

template <typename T>
T *ThreadT<T>::TaskInf::getT()
{
    return arg;
}

template <typename T>
std::function<int32_t (T *)> ThreadT<T>::TaskInf::getFunc()
{
    return func;
}

template <typename T>
typename ThreadT<T>::thread_control_cmd ThreadT<T>::TaskInf::getType()
{
    return type;
}

template <typename T>
SyncType *ThreadT<T>::TaskInf::getSync()
{
    return sync;
}

template <typename T>
void ThreadT<T>::TaskInf::set(thread_control_cmd t, bool buffered)
{
    type = t;
    bufMe = buffered;
}

template <typename T>
void ThreadT<T>::TaskInf::set(std::function<int32_t (T *)> f, T *p, bool buffered)
{
    func = f;
    arg = p;
    bufT = buffered;
}

template <typename T>
void ThreadT<T>::TaskInf::set(SyncType *s, bool buffered)
{
    sync = s;
    bufSync = buffered;
}

template <typename T>
void ThreadT<T>::TaskInf::clear()
{
    bufMe = false;
    bufT  = false;
    bufSync = false;
    rc = NO_ERROR;
    type = THREAD_CONTROL_EXIT_THREAD;
    sync = NULL;
    arg = NULL;
}

template <typename T>
uint32_t ThreadT<T>::TaskInf::id()
{
    return index;
}

};


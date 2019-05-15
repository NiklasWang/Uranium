#include "MemMgmt.h"
#include "Thread.h"

namespace sirius {

int32_t Thread::run(std::function<int32_t ()> func,
    std::function<int32_t (Thread *)> cb)
{
    InternalTask task;

    return run<InternalTask>(
        [func](InternalTask * /*task*/) -> int32_t {
            return func();
        },
        &task,
        cb);
}

int32_t Thread::runWait(std::function<int32_t ()> func,
    std::function<int32_t (Thread *)> cb)
{
    InternalTask task;

    return runWait<InternalTask>(
        [func](InternalTask * /*task*/) -> int32_t {
            return func();
        },
        &task,
        cb);
}

int32_t Thread::run(std::function<int32_t ()> func, sync_type sync,
    std::function<int32_t (Thread *)> cb)
{
    int32_t rc = NO_ERROR;

    if (sync == SYNC_TYPE) {
        rc = runWait(func, cb);
    }
    if (sync == ASYNC_TYPE) {
        rc = run(func, cb);
    }

    return rc;
}

Thread::Thread(std::string name) :
    mName(name),
    mModule(MODULE_THREAD_POOL),
    mTid(0),
    mTaskCnt(0),
    mStatus(THREAD_STATUS_UNINITED)
{
}

Thread::~Thread()
{
    if (mStatus != THREAD_STATUS_UNINITED) {
        destruct();
    }
}

void *Thread::runThread(void *thiz)
{
    assert(thiz != NULL);

    Thread *pme = static_cast<Thread *>(thiz);
    pme->threadLoop();

    return NULL;
}

int32_t Thread::threadLoop()
{
    int32_t rc = NO_ERROR;
    bool exit = false;
    TaskBase *task = NULL;

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
                rc = callFunc(task, ISNULL(task->getSync()));
                utils_atomic_dec(&mTaskCnt);
                task->rc = rc;
                task->cb(this);
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

        if (NOTNULL(task->getSync())) {
            task->getSync()->signal();
        } else {
            SECURE_DELETE(task);
        }

        mStatus = THREAD_STATUS_RUNNING;
    } while(!exit);

    mStatus = THREAD_STATUS_INITED;
    LOGD(mModule, "X Thread %s exited", whoamI());
    return rc;
}

int32_t Thread::construct()
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

    if (SUCCEED(rc)) {
        LOGI(mModule, "Thread %s constructed.", whoamI());
    }

    return rc;
}

int32_t Thread::destruct()
{
    int32_t rc = NO_ERROR;
    TaskBase *task  = NULL;

    if (mStatus == THREAD_STATUS_UNINITED) {
        LOGE(mModule, "already exit");
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        task = new TaskBase();
        if (ISNULL(task)) {
            LOGE(mModule, "Failed to alloc buffered task info or new");
            rc = NO_MEMORY;
        } else {
            task->clear();
            task->set(THREAD_CONTROL_EXIT_THREAD);
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
        TaskBase *iter = NULL;
        while(NOTNULL(iter = mQ.dequeue())) {
            if (iter->getSync()) {
                if (!iter->bufSync && NOTNULL(iter->getSync())) {
                    SyncType *sync = iter->getSync();
                    SECURE_DELETE(sync);
                }
                if (iter->bufSync && NOTNULL(iter->getSync())) {
                    PUT_OBJ_TO_BUFFER(mSyncBuf, iter->getSync());
                }
            }
            SECURE_DELETE(iter);
        }
        mQ.flush();
        mSyncBuf.flush();
    }

    if (SUCCEED(rc)) {
        LOGD(mModule, "Thread %s destructed.", whoamI());
    }

    return rc;
}

bool Thread::idle()
{
    return !busy();
}

bool Thread::busy()
{
    return mTaskCnt;
}

int32_t Thread::workload()
{
    return mTaskCnt;
}

const char *Thread::whoamI()
{
    return mName.c_str();
}

uint32_t Thread::TaskBase::cnt = 0;

Thread::TaskBase::TaskBase() :
    bufSync(false),
    rc(NO_ERROR),
    type(THREAD_CONTROL_EXIT_THREAD),
    sync(NULL),
    index(cnt++)
{}

Thread::TaskBase::TaskBase(const Thread::TaskBase &rhs)
{
    if (this != &rhs) {
        *this = rhs;
        index = cnt++;
    }
}

Thread::thread_control_cmd Thread::TaskBase::getType()
{
    return type;
}

SyncType *Thread::TaskBase::getSync()
{
    return sync;
}

void Thread::TaskBase::set(thread_control_cmd t)
{
    type = t;
}

void Thread::TaskBase::set(SyncType *s, bool buffered)
{
    sync = s;
    bufSync = buffered;
}

void Thread::TaskBase::clear()
{
    bufSync = false;
    rc = NO_ERROR;
    type = THREAD_CONTROL_EXIT_THREAD;
    sync = NULL;
}

uint32_t Thread::TaskBase::id()
{
    return index;
}

Thread::InternalTask::InternalTask() :
    module(MODULE_THREAD_POOL)
{
}

ModuleType Thread::InternalTask::getModule()
{
    return module;
}

};


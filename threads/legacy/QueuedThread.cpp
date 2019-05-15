#include "QueuedThread.h"

namespace sirius {

QueuedThread::QueuedThread(String8 name) :
    mThreadName(name),
    mTid(0),
    mSem(NULL),
    mQ(NULL),
    mStatus(THREAD_STATUS_UNINITED),
    mModule(MODULE_THREAD_POOL)
{
}

QueuedThread::~QueuedThread()
{
    if (mStatus != THREAD_STATUS_UNINITED) {
        destructThread();
    }
}

void *QueuedThread::runThread(void *dat)
{
    assert(dat != NULL);

    QueuedThread *pme = static_cast<QueuedThread *>(dat);
    pme->threadLoop();

    return NULL;
}

int32_t QueuedThread::threadLoop()
{
    int32_t rc = NO_ERROR;
    bool exit = false;
    thread_control_dat *cmd = NULL;

    LOGD(mModule, "E Thread %s Created", getThreadName());
    pthread_mutex_unlock(&mReadyLock);

    do {
        do {
            rc = mSem->wait();
            if (rc != 0 && errno != EINVAL) {
                LOGE(mModule, "cam_sem_wait error [%s]", strerror(errno));
            }
        } while (rc != 0);

        mStatus = THREAD_STATUS_WORKING;
        cmd = static_cast<thread_control_dat *>(mQ->dequeue());
        if (ISNULL(cmd)) {
            LOGE(mModule, "Dequeue failed");
            mStatus = THREAD_STATUS_RUNNING;
            continue;
        }

        switch (cmd->type) {
            case THREAD_CONTROL_NEW_JOB: {
                rc = processTask(cmd->dat);
                if (!SUCCEED(rc)) {
                    LOGI(mModule, "Process task abnormal, may okay");
                }
                taskDone(cmd->dat, rc);
            } break;
            case THREAD_CONTROL_EXIT_THREAD: {
                exit = true;
            } break;
            default: {
                LOGE(mModule, "Oops, received unknown cmd, ignore");
            } break;
        }

        cmd->dat = NULL;
        free(cmd);

        mStatus = THREAD_STATUS_RUNNING;
    } while(!exit);

    mStatus = THREAD_STATUS_INITED;
    LOGD(mModule, "X Thread %s Exit", getThreadName());
    return rc;
}

int32_t QueuedThread::newTask(void *task)
{
    int32_t rc = NO_ERROR;
    thread_control_dat *cmd = NULL;

    assert(task != NULL);
    if (mStatus == THREAD_STATUS_UNINITED) {
        LOGE(mModule, "already exit");
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        cmd = (thread_control_dat *)malloc(sizeof(thread_control_dat));
        cmd->type = THREAD_CONTROL_NEW_JOB;
        cmd->dat = task;
        if (ISNULL(mQ) || ISNULL(mSem)) {
            LOGE(mModule, "Failed to get command queue");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (!mQ->enqueue(cmd)) {
            LOGE(mModule, "Failed to enqueue");
            rc = UNKNOWN_ERROR;
        } else {
            mSem->signal();
        }
    }

    return rc;
}

int32_t QueuedThread::constructThread()
{
    int32_t rc = NO_ERROR;

    if (mStatus != THREAD_STATUS_UNINITED) {
        LOGE(mModule, "thread exists");
        rc = ALREADY_EXISTS;
    }

    if (SUCCEED(rc)) {
        mSem = new Semaphore();
        if (ISNULL(mSem)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        mQ = new CQueue();
        if (ISNULL(mQ)) {
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        mStatus = THREAD_STATUS_INITED;
        pthread_mutex_init(&mReadyLock, NULL);
        pthread_mutex_lock(&mReadyLock);

        if (pthread_create(&mTid, NULL, runThread, this)) {
            LOGE(mModule, "Failed to create working thread");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        pthread_setname_np(mTid, mThreadName.string());
        pthread_mutex_lock(&mReadyLock);
        mStatus = THREAD_STATUS_RUNNING;
        pthread_mutex_unlock(&mReadyLock);
    }

    if (!SUCCEED(rc) && rc != ALREADY_EXISTS) {
        LOGE(mModule, "Failed to start thread");
        if (!ISNULL(mQ)) {
            mQ->flush();
            delete mQ;
            mQ = NULL;
        }
        if (!ISNULL(mSem)) {
            delete mSem;
            mSem = NULL;
        }
        mStatus = THREAD_STATUS_UNINITED;
        pthread_mutex_destroy(&mReadyLock);
        mTid = 0;
    }

    return rc;
}

int32_t QueuedThread::destructThread()
{
    int32_t rc = NO_ERROR;
    thread_control_dat *cmd = NULL;

    if (mStatus == THREAD_STATUS_UNINITED) {
        LOGE(mModule, "already exit");
        rc = NOT_INITED;
    }

    if (SUCCEED(rc)) {
        cmd = (thread_control_dat *)malloc(sizeof(thread_control_dat));
        cmd->type = THREAD_CONTROL_EXIT_THREAD;
        cmd->dat = NULL;
        if (ISNULL(mQ) || ISNULL(mSem)) {
            LOGE(mModule, "failed to get command queue");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (!mQ->enqueue(cmd)) {
            LOGE(mModule, "Failed to enqueue");
            rc = UNKNOWN_ERROR;
        } else {
            mSem->signal();
        }
    }

    if (SUCCEED(rc)) {
        if (pthread_join(mTid, NULL)) {
            LOGE(mModule, "failed to stop thread");
            rc = UNKNOWN_ERROR;
        }
    }

    if (SUCCEED(rc)) {
        if (!ISNULL(mQ)) {
            thread_control_dat *cmd = NULL;
            while((cmd = static_cast<thread_control_dat *>(
                mQ->dequeue()))) {
                free(cmd);
            }
            mQ->flush();
            delete mQ;
            mQ = NULL;
        }
    }

    if (SUCCEED(rc)) {
        if (!ISNULL(mSem)) {
            delete mSem;
            mSem = NULL;
        }
        mStatus = THREAD_STATUS_UNINITED;
        pthread_mutex_destroy(&mReadyLock);
        mTid = 0;
    }

    return rc;
}

const char *QueuedThread::getThreadName()
{
    return mThreadName.string();
}

};

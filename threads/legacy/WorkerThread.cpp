#include "WorkerThread.h"

namespace sirius {

WorkerThread::WorkerThread(
    ThreadIntf *parent,
    const char *threadName) :
    QueuedThread(String8(threadName)),
    mConstructed(false),
    mTaskCnt(0),
    mParent(parent),
    mModule(MODULE_THREAD_POOL)
{
}

WorkerThread::~WorkerThread()
{
    if (mConstructed) {
        destruct();
    }
}

int32_t WorkerThread::construct()
{
    RWLock::AutoWLock l(mLock);
    if (SUCCEED(constructThread())) {
        mConstructed = true;
    }
    return mConstructed ? NO_ERROR : UNKNOWN_ERROR;
}

int32_t WorkerThread::destruct()
{
    RWLock::AutoWLock l(mLock);
    mConstructed = false;
    return destructThread();
}

bool WorkerThread::busy()
{
    return mTaskCnt;
}

int32_t WorkerThread::newTask(void *task)
{
    ATOMIC_INC(&mTaskCnt);
    nodityListener(WORKER_STATUS_BUSY);
    return QueuedThread::newTask(task);
}

int32_t WorkerThread::registerListener(IStatusListener *listener)
{
    if (NOTNULL(listener)) {
        mListener.push_back(listener);
    }

    return NO_ERROR;
}


int32_t WorkerThread::processTask(void *dat)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mParent)) {
        LOGE(mModule, "Attach thread not inited");
        rc = NOT_INITED;
    } else {
        rc = mParent->processTask(dat);
    }

    return rc;
}

int32_t WorkerThread::taskDone(void *dat, int32_t bottom)
{
    int32_t rc = NO_ERROR;

    if (ISNULL(mParent)) {
        LOGE(mModule, "Attach thread not inited");
        rc = NOT_INITED;
    } else {
        rc = mParent->taskDone(dat, bottom);
    }

    nodityListener(WORKER_STATUS_IDLE);
    ATOMIC_DEC(&mTaskCnt);
    return rc;
}

int32_t WorkerThread::nodityListener(WorkerStatus status)
{
    for (auto &listener : mListener) {
        if (NOTNULL(listener)) {
            listener->onStatusChanged(status);
        }
    }

    return NO_ERROR;
}

};


#ifndef _QUEUED_THREAD_H_
#define _QUEUED_THREAD_H_

#include "common.h"
#include "Semaphore.h"
#include "CQueue.h"
#include "ThreadIntf.h"

namespace sirius {

class QueuedThread :
    public ThreadIntf
{
public:
    QueuedThread(String8 name = String8("noname_thread"));
    virtual ~QueuedThread();
    static void *runThread(void *dat);
    int32_t threadLoop();
    int32_t newTask(void *task);
    int32_t constructThread();
    int32_t destructThread();
    const char *getThreadName();

public:
    virtual int32_t processTask(void *dat) = 0;
    virtual int32_t taskDone(void *dat, int32_t rc) = 0;

private:
    enum thread_status {
        THREAD_STATUS_UNINITED,
        THREAD_STATUS_INITED,
        THREAD_STATUS_RUNNING,
        THREAD_STATUS_WORKING,
    };

    enum thread_control_cmd {
        THREAD_CONTROL_NEW_JOB,
        THREAD_CONTROL_EXIT_THREAD,
    };

    struct thread_control_dat {
        thread_control_cmd type;
        void *dat;
    };

private:
    QueuedThread(const QueuedThread &);
    QueuedThread& operator=(const QueuedThread &rhs);

private:
    String8          mThreadName;
    pthread_t        mTid;
    Semaphore       *mSem;
    CQueue          *mQ;
    thread_status    mStatus;
    pthread_mutex_t  mReadyLock;
    ModuleType       mModule;
};

};

#endif

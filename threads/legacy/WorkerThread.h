#ifndef _WORKER_THREAD_H_
#define _WORKER_THREAD_H_

#include "common.h"
#include "ThreadIntf.h"
#include "QueuedThread.h"

namespace sirius {

enum WorkerStatus {
    WORKER_STATUS_BUSY,
    WORKER_STATUS_IDLE,
};

class IStatusListener {
public:
     virtual int32_t onStatusChanged(WorkerStatus status) = 0;
     virtual ~IStatusListener() = default;
};

class WorkerThread :
    public QueuedThread,
    public RefBase {
public:
    int32_t construct();
    int32_t destruct();
    bool busy();
    int32_t newTask(void *task);
    int32_t registerListener(IStatusListener *listener);

public:
    explicit WorkerThread(ThreadIntf *parent = NULL,
        const char *threadName = "WorkerThread");
    ~WorkerThread();

private:
    int32_t nodityListener(WorkerStatus status);
    virtual int32_t processTask(void *dat) override;
    virtual int32_t taskDone(void *dat, int32_t rc) override;

private:
    WorkerThread(const WorkerThread &rhs) = delete;
    WorkerThread &operator=(const WorkerThread &rhs) = delete;

private:
    bool         mConstructed;
    int32_t      mTaskCnt;
    RWLock       mLock;
    ThreadIntf  *mParent;
    ModuleType   mModule;
    List<IStatusListener *> mListener;
};

};

#endif

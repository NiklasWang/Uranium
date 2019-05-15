#ifndef _RUN_ONCE_THREAD_H_
#define _RUN_ONCE_THREAD_H_

#include "SyncType.h"
#include "QueuedThread.h"

namespace sirius {

class RunOnceFunc {
public:
    virtual int32_t runOnceFunc(void *in, void *out) = 0;
    virtual int32_t onOnceFuncFinished(int32_t rc) = 0;
    virtual int32_t abortOnceFunc() = 0;
    virtual ~RunOnceFunc() {}
};

class RunOnceThread :
    public QueuedThread,
    public noncopyable {
public:
    int32_t run(RunOnceFunc *func, void *in, void *out);
    int32_t exit();
    bool isRuning();

private:
    int32_t processTask(void *dat) override;
    int32_t taskDone(void *dat, int32_t rc) override;

public:
    RunOnceThread();
    virtual ~RunOnceThread();

private:
    bool         mRuning;
    bool         mExited;
    SyncType     mSync;
    ModuleType   mModule;
    int32_t      mRc;
    RunOnceFunc *mFunc;
    void        *mIn, *mOut;
};

};

#endif

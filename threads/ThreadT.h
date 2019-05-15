#ifndef __THREADT_H_
#define __THREADT_H_

#include <pthread.h>
#include <string>
#include <functional>

#include "common.h"
#include "Semaphore.h"
#include "QueueT.h"
#include "SyncType.h"
#include "ObjectBuffer.h"

namespace sirius {

template <typename T = int32_t>
class ThreadT {
public:
    int32_t run(std::function<int32_t ()> func);
    int32_t run(std::function<int32_t (T *)> func, T *arg);
    int32_t runWait(std::function<int32_t ()> func);
    int32_t runWait(std::function<int32_t (T *)> func, T *arg);
    int32_t run(std::function<int32_t ()> func, sync_type sync);
    int32_t run(std::function<int32_t (T *)> func, T *arg, sync_type sync);

    const char *whoamI();

    int32_t construct();
    int32_t destruct();

public:
    ThreadT(std::string name = "noname_thread");
    virtual ~ThreadT();
    static void *runThread(void *thiz);
    int32_t threadLoop();

private:
    int32_t run(std::function<int32_t (T *)> func,
        T *arg, sync_type sync, bool internal);

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

    struct TaskInf {
    public:
        T *getT();
        std::function<int32_t (T *)> getFunc();
        SyncType *getSync();
        thread_control_cmd getType();
        void set(thread_control_cmd type, bool bufferMe);
        void set(std::function<int32_t (T *)> func, T *p, bool buffered);
        void set(SyncType *s, bool buffered);
        void clear();
        uint32_t id();
        TaskInf();
        TaskInf(const TaskInf &rhs);

    public:
        bool bufMe;
        bool bufT;
        bool bufSync;
        int32_t rc;

    private:
        thread_control_cmd type;
        SyncType *sync;
        T *arg;
        std::function<int32_t (T *)> func;

        uint32_t index;
        static uint32_t cnt;
    };

private:
    ThreadT(const ThreadT &);
    ThreadT& operator=(const ThreadT &rhs);

private:
    std::string   mName;
    pthread_t     mTid;
    Semaphore     mSem;
    thread_status mStatus;
    Semaphore     mReadySignal;
    ModuleType    mModule;

    QueueT<TaskInf>        mQ;
    ObjectBuffer<TaskInf>  mTaskBuf;
    ObjectBuffer<T>        mArgBuf;
    ObjectBuffer<SyncType> mSyncBuf;
};

};

#include "ThreadT.hpp"

#endif

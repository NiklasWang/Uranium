#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>
#include <string>
#include <functional>

#include "common.h"
#include "Semaphore.h"
#include "QueueT.h"
#include "SyncType.h"
#include "ObjectBuffer.h"

namespace sirius {

class Thread :
    virtual public noncopyable {

public:

    int32_t run(std::function<int32_t ()> func,
        std::function<int32_t (Thread *)> cb = [](Thread *) -> int32_t { return 0; });

    template <typename T>
    int32_t run(std::function<int32_t (T *)> func, T *arg,
        std::function<int32_t (Thread *)> cb = [](Thread *) -> int32_t { return 0; });

    int32_t runWait(std::function<int32_t ()> func,
        std::function<int32_t (Thread *)> cb = [](Thread *) -> int32_t { return 0; });

    template <typename T>
    int32_t runWait(std::function<int32_t (T *)> func, T *arg,
        std::function<int32_t (Thread *)> cb = [](Thread *) -> int32_t { return 0; });

    int32_t run(std::function<int32_t ()> func, sync_type sync,
        std::function<int32_t (Thread *)> cb = [](Thread *) -> int32_t { return 0; });

    template <typename T>
    int32_t run(std::function<int32_t (T *)> func, T *arg, sync_type sync,
        std::function<int32_t (Thread *)> cb = [](Thread *) -> int32_t { return 0; });

    bool idle();
    bool busy();
    int32_t workload();
    const char *whoamI();

private:
    template <typename T>
    int32_t run(std::function<int32_t (T *)> func, T *arg,
        sync_type sync, std::function<int32_t (Thread *)> cb, bool internal);
    struct TaskBase;
    int32_t callFunc(TaskBase *task, bool release); // TODO: This is temp solution, fix later

public:
    Thread(std::string name = "noname_thread");
    virtual ~Thread();
    int32_t construct();
    int32_t destruct();

    static void *runThread(void *thiz);
    int32_t threadLoop();

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

    struct InternalTask {
        ModuleType module;
        ModuleType getModule();
        InternalTask();
    };

    struct TaskBase {
    public:
        SyncType *getSync();
        thread_control_cmd getType();
        void set(thread_control_cmd type);
        void set(SyncType *s, bool buffered);
        void clear();
        uint32_t id();
        TaskBase();
        TaskBase(const TaskBase &rhs);
        virtual ~TaskBase() {}

    public:
        bool bufSync;
        int32_t rc;
        ModuleType module;
        std::function<int32_t (Thread *)> cb;

    private:
        thread_control_cmd type;
        SyncType *sync;

        uint32_t index;
        static uint32_t cnt;
    };

    template <typename T>
    struct TaskInf :
        public TaskBase {
    public:
        T *arg;
        std::function<int32_t (T *)> func;

        TaskInf();
        TaskInf(const TaskInf &);
    };

private:
    std::string   mName;
    ModuleType    mModule;
    pthread_t     mTid;
    int32_t       mTaskCnt;
    Semaphore     mSem;
    thread_status mStatus;
    Semaphore     mReadySignal;

    QueueT<TaskBase>       mQ;
    ObjectBuffer<SyncType> mSyncBuf;
};

};

#include "Thread.hpp"

#endif

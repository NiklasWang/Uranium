#include "ThreadPool.h"

namespace sirius {

template <typename T>
int32_t ThreadPool::run(std::function<int32_t (T *)> func, T *arg)
{
    return run<T>(func, arg, ASYNC_TYPE);
}

template <typename T>
int32_t ThreadPool::runWait(std::function<int32_t (T *)> func, T *arg)
{
    return run<T>(func, arg, SYNC_TYPE);
}

template <typename T>
int32_t ThreadPool::run(std::function<int32_t (T *)> func, T *arg, sync_type sync)
{
    int32_t rc = NO_ERROR;
    WorkerThread *worker = get(
        sync == SYNC_TYPE ? BLOCK_TYPE : NONBLOCK_TYPE);

    if (NOTNULL(worker)) {
        rc = worker->run<T>(func, arg, sync,
            [this](Thread *thread) -> int32_t {
                return callback(thread);
            }
        );
        if (!SUCCEED(rc)) {
            LOGD(mModule, "Failed to run on worker thread, %d", rc);
        }
    } else {
        LOGE(mModule, "Failed to find idle thread or create new one");
        rc = UNKNOWN_ERROR;
    }

    return rc;
}

};


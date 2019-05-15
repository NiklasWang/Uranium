#include "ThreadPoolEx.h"

namespace sirius {

uint32_t ThreadPoolEx::gCnt = 0;

ThreadPoolEx *ThreadPoolEx::gThis = NULL;

pthread_mutex_t ThreadPoolEx::mInsL = PTHREAD_MUTEX_INITIALIZER;

ModuleType ThreadPoolEx::gModule = MODULE_THREAD_POOL;

ThreadPoolEx *ThreadPoolEx::getInstance()
{
    pthread_mutex_lock(&mInsL);

    if (ISNULL(gThis)) {
        gThis = new ThreadPoolEx(0);
        if (ISNULL(gThis)) {
            LOGE(gModule, "Failed to create thread pool ex.");
        }
    }

    if (NOTNULL(gThis)) {
        gCnt++;
    }

    pthread_mutex_unlock(&mInsL);

    return gThis;
}

uint32_t ThreadPoolEx::removeInstance()
{
    ASSERT_LOG(gModule, gCnt > 0, "Instance not got.");
    ASSERT_LOG(gModule, NOTNULL(gThis), "Instance not created.");

    pthread_mutex_lock(&mInsL);

    gCnt--;
    if (gCnt == 0 && NOTNULL(gThis)) {
        delete gThis;
        gThis = NULL;
    }

    pthread_mutex_unlock(&mInsL);

    return gCnt;
}

ThreadPoolEx::ThreadPoolEx(uint32_t c) :
    ThreadPool(c)
{}

ThreadPoolEx::~ThreadPoolEx()
{
    LOGI(gModule, "Thread pool will be destructed, %d threads", workload());
}

};


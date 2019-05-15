#include "SemaphoreTimeout.h"

namespace sirius {

int SemaphoreTimeout::wait()
{
    int rc = 0;
    struct timespec ts;
    pthread_mutex_lock(&mutex);
    while (cnt == 0) {
        if (mTime == 0) {
            rc = pthread_cond_wait(&cond, &mutex);
        } else {
            clock_gettime(CLOCK_MONOTONIC, &ts);
            int64_t nsec = ts.tv_nsec + 1000000LL * mTime;
            ts.tv_sec += nsec / 1000000000;
            ts.tv_nsec = nsec % 1000000000;
            rc = pthread_cond_timedwait(&cond, &mutex, &ts);
            if (rc) {
                cnt++;
            }
        }
    }
    cnt--;
    pthread_mutex_unlock(&mutex);
    return rc;

}

bool SemaphoreTimeout::needWait()
{
    return cnt == 0;
}

SemaphoreTimeout::SemaphoreTimeout(int32_t timeout, int n) :
    Semaphore(n),
    mTime(timeout)
{
}

SemaphoreTimeout::~SemaphoreTimeout()
{
}

};


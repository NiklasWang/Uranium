#include "Semaphore.h"

namespace sirius {

Semaphore::Semaphore(int n)
{
    pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setclock(&cond_attr, CLOCK_MONOTONIC);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, &cond_attr);

    pthread_condattr_destroy(&cond_attr);
    cnt = n;
}

Semaphore::~Semaphore()
{
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    cnt = 0;
}

void Semaphore::signal()
{
    pthread_mutex_lock(&mutex);
    cnt++;
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
}

int Semaphore::wait()
{
    int rc = 0;
    pthread_mutex_lock(&mutex);
    while (cnt == 0)
        rc = pthread_cond_wait(&cond, &mutex);
    cnt--;
    pthread_mutex_unlock(&mutex);
    return rc;
}

};


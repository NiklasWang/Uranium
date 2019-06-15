#include "common.h"
#include "TimedSemaphore.h"

namespace uranium {

void TimedSemaphore::signal()
{
    mutex.lock();
    cnt++;
    cond.wakeOne();
    mutex.unlock();
}

int TimedSemaphore::wait()
{
    bool timeout = false;

    mutex.lock();
    while (cnt == 0) {
        timeout = !cond.wait(&mutex, ms);
        if (timeout) {
            break;
        }
    }
    if (!timeout) {
        cnt--;
    }
    mutex.unlock();

    return timeout ? TIMEDOUT : NO_ERROR;
}

bool TimedSemaphore::needWait()
{
    return cnt == 0;
}

int32_t TimedSemaphore::time()
{
    return ms;
}

TimedSemaphore::TimedSemaphore(int32_t ms, int n) :
    ms(ms),
    cnt(n)
{
}

TimedSemaphore::~TimedSemaphore()
{
    cnt = 0;
}

}


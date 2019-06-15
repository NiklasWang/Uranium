#ifndef _SEMAPHORE_TIMEOUT_H_
#define _SEMAPHORE_TIMEOUT_H_

#include <stdint.h>

#include <QMutex>
#include <QWaitCondition>

namespace uranium {

class TimedSemaphore
{
public:
    int wait();
    void signal();
    bool needWait();
    int32_t time();

public:
    TimedSemaphore(int32_t timeout, int n = 0);
    virtual ~TimedSemaphore();

private:
    TimedSemaphore(const TimedSemaphore &rhs) = delete;
    TimedSemaphore &operator=(const TimedSemaphore &rhs) = delete;

private:
    int32_t ms;
    int32_t cnt;
    QMutex  mutex;
    QWaitCondition cond;
};

}

#endif

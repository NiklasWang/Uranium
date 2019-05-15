#ifndef _SEMAPHORE_TIMEOUT_H_
#define _SEMAPHORE_TIMEOUT_H_

#include "Semaphore.h"

namespace sirius {

class SemaphoreTimeout :
    public Semaphore {
public:
    int wait();
    bool needWait();

public:
    SemaphoreTimeout(int32_t timeout, int n = 0);
    virtual ~SemaphoreTimeout();

private:
    SemaphoreTimeout(const SemaphoreTimeout &rhs) = delete;
    SemaphoreTimeout &operator=(const SemaphoreTimeout &rhs) = delete;

private:
    int32_t mTime;
};

};

#endif

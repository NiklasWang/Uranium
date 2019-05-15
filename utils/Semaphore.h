#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <pthread.h>

namespace sirius {

enum BlockType {
    BLOCK_TYPE,
    NONBLOCK_TYPE,
};

class Semaphore {
public:
    void signal();
    int wait();

public:
    Semaphore(int n = 0);
    virtual ~Semaphore();

private:
    Semaphore(const Semaphore &rhs) = delete;
    Semaphore &operator=(const Semaphore &rhs) = delete;

protected:
    volatile int cnt;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};

};

#endif /* _SEMAPHORE_H_ */

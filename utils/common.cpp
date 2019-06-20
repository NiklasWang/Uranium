#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>

namespace uranium {

int64_t getThreadId()
{
    pthread_t id = pthread_self();
    return id;
}

};

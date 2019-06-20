#include <pthread.h>

#include "common.h"

namespace uranium {

#ifdef _CYGWIN_COMPILE_

int64_t getThreadId()
{
    pthread_t id = pthread_self();
    return (int64_t)id;
}

#else

int64_t getThreadId()
{
    return pthread_self();
}

#endif

};

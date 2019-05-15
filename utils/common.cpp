#include <stdio.h>
#include <string.h>
#include <pthread.h>

int64_t getThreadId()
{
    char str[sizeof(pthread_t) + 3] = { '\0' };
    char tmp[3] = { '\0' };
    pthread_t id = pthread_self();
    unsigned char *pid = (unsigned char*)(void*)(&id);
    int64_t result = 0LL;

    strncat(str, "0x", sizeof(str));
    for (size_t i=0; i < sizeof(pthread_t); i++) {
        sprintf(tmp, "%02x", (unsigned)(pid[i]));
        tmp[sizeof(tmp) - 1] = '\0';
        strncat(str, tmp, sizeof(tmp));
    }
    
    sscanf(str, "%x", &result);

    return result;
}
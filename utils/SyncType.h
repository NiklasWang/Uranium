#ifndef _SYNC_TYPE_H_
#define _SYNC_TYPE_H_

#include "Semaphore.h"

namespace sirius {

enum sync_type {
    SYNC_TYPE,
    ASYNC_TYPE,
    SYNC_TYPE_MAX_INVALID,
};

class SyncType {
public:
    int32_t wait();
    void signal();

public:
    SyncType(sync_type type = SYNC_TYPE);
    ~SyncType();
    SyncType &operator=(const sync_type &rhs);
    SyncType &operator=(const SyncType &rhs);
    bool operator==(const sync_type &rhs);
    bool operator==(const SyncType &rhs);
    bool operator!=(const sync_type &rhs);
    bool operator!=(const SyncType &rhs);
    static const char *whoami(sync_type type);

private:
    sync_type  mType;
    Semaphore *mSem;
    static const char * const kTypeString[];
};

};

#endif

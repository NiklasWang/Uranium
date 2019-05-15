#include "SyncType.h"
#include "common.h"

namespace sirius {

const char * const SyncType::kTypeString[] = {
    [SYNC_TYPE]  = "sync",
    [ASYNC_TYPE] = "async",
};

SyncType::SyncType(sync_type type) :
    mType(type),
    mSem(NULL)
{
    if (type == SYNC_TYPE) {
        mSem = new Semaphore();
    }
}

SyncType::~SyncType()
{
    if (NOTNULL(mSem)) {
        SECURE_DELETE(mSem);
    }
}

int32_t SyncType::wait()
{
    int32_t rc = NOT_INITED;

    if (mType == SYNC_TYPE) {
        assert(NOTNULL(mSem));
        rc = mSem->wait();
    }

    return rc;
}

void SyncType::signal()
{
    if (mType == SYNC_TYPE) {
        assert(NOTNULL(mSem));
        mSem->signal();
    }

    return;
}

SyncType &SyncType::operator=(const sync_type &rhs)
{
    if (*this != rhs) {
        if (mType == SYNC_TYPE && rhs == ASYNC_TYPE) {
            mType = rhs;
            SECURE_DELETE(mSem);
        } else if (mType == ASYNC_TYPE && rhs == SYNC_TYPE) {
            mType = rhs;
            mSem = new Semaphore();
        } else if (mType == SYNC_TYPE && rhs == SYNC_TYPE) {
            LOGE(MODULE_UTILS, "Forbidden to override a sync type");
        }
    }

    return *this;
}

SyncType &SyncType::operator=(const SyncType &rhs)
{
    return *this = rhs.mType;
}

bool SyncType::operator== (const sync_type &rhs)
{
    return mType == rhs;
}

bool SyncType::operator==(const SyncType &rhs)
{
    return mType == rhs.mType;
}

bool SyncType::operator!=(const sync_type &rhs)
{
    return !(*this == rhs);
}

bool SyncType::operator!=(const SyncType &rhs)
{
    return !(*this == rhs);
}

const char *SyncType::whoami(sync_type type)
{
    return kTypeString[type];
}

};


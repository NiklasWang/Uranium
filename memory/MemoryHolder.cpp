#include "common.h"
#include "MemoryHolder.h"
#include "MemMgmt.h"

namespace sirius {

int32_t MemoryHolder::add(int32_t id, void *dat, int32_t size, bool copy)
{
    int32_t rc = NO_ERROR;
    void *copied = NULL;

    ASSERT_LOG(mModule, id >= 0, "id %d not valid.", id);
    ASSERT_LOG(mModule, NOTNULL(dat), "data is NULL.");
    ASSERT_LOG(mModule, size > 0, "size %d not valid.", size);

    if (SUCCEED(rc)) {
        if (!copy) {
            rc = add(id, dat, size);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to add to holder, %d", rc);
            } else {
                rc = JUMP_DONE;
            }
        }
    }

    if (SUCCEED(rc)) {
        copied = Malloc(size);
        if (ISNULL(copied)) {
            LOGE(mModule, "Failed to malloc %d bytes", size);
            rc = NO_MEMORY;
        } else {
            memcpy(copied, dat, size);
        }
    }

    if (SUCCEED(rc)) {
        rc = add(id, copied, size);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to add to holder, %d", rc);
        }
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t MemoryHolder::add(int32_t id, void *dat, int32_t size)
{
    int32_t rc = NOT_FOUND;

    Block block = {
        .id = id,
        .size = size,
        .data = dat,
    };

    auto iter = mQ.begin();
    for (iter = mQ.begin(); iter != mQ.end(); iter++) {

        if (iter->id == id) {
            if (COMPARE_SAME_DATA(iter->data, dat, size)) {
                LOGI(mModule, "Holder %d id %d already exists,"
                    "And data is the same.", getType(), id);
                rc = ALREADY_EXISTS;
            } else {
                LOGE(mModule, "Holder %d id %d already exists,"
                    "But data is different.",getType(), id);
                rc = INVALID_FORMAT;
            }
            break;
        }

        if(id < iter->id){
            if (mThreadSafe) {
                RWLock::AutoWLock l(mLock);
                mQ.insert(iter, block);
            } else {
                mQ.insert(iter, block);
            }
            rc = NO_ERROR;
            break;
        }
    }

    if(iter == mQ.end()){
        if (mThreadSafe) {
            RWLock::AutoWLock l(mLock);
            mQ.push_back(block);
        } else {
            mQ.push_back(block);
        }
        rc = NO_ERROR;
    }


    if (SUCCEED(rc)) {
        if (NOTNULL(mCb) && isCompleted()) {
            void *dat = NULL;
            int32_t size = 0;
            rc = flatten(&dat, &size);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to flatten, %d", rc);
            } else {
                rc = mCb(getType(), dat, size);
                if (!SUCCEED(rc)) {
                    LOGE(mModule, "Failed to call cb func, %d", rc);
                }
            }
        }
    }

    return rc;
}

int32_t MemoryHolder::flatten(void **dat, int32_t *size)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (NOTNULL(mFlatten)) {
            *dat  = mFlatten;
            *size = mFlattenSize;
            rc = JUMP_DONE;
        }
    }

    if (SUCCEED(rc)) {
        if (!isCompleted()) {
            LOGE(mModule, "Data not completed, can't flattern.");
            rc = NOT_READY;
        }
    }

    if (SUCCEED(rc)) {
        mFlattenSize = 0;
        for (auto iter : mQ) {
            mFlattenSize += iter.size;
        }
    }

    if (SUCCEED(rc)) {
        mFlatten = Malloc(mFlattenSize);
        if (ISNULL(mFlatten)) {
            LOGE(mModule, "Failed to alloc %d bytes.", mFlattenSize);
            mFlattenSize = 0;
            rc = NO_MEMORY;
        }
    }

    if (SUCCEED(rc)) {
        int32_t copied = 0;
        int8_t *_data = (int8_t *)mFlatten;
        for (auto iter : mQ) {
            memcpy(_data + copied, iter.data, iter.size);
            copied += iter.size;
        }
    }

    if (SUCCEED(rc)) {
        *dat  = mFlatten;
        *size = mFlattenSize;
    }

    return RETURNIGNORE(rc, JUMP_DONE);
}

int32_t MemoryHolder::flush()
{
    for (auto iter : mQ) {
        if (NOTNULL(iter.data)) {
            SECURE_FREE(iter.data);
        }
    }

    if (mThreadSafe) {
        RWLock::AutoWLock l(mLock);
        mQ.clear();
    } else {
        mQ.clear();
    }

    return NO_ERROR;
}

int32_t MemoryHolder::isCompleted()
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (mBlocks) {
            if (mQ.size() != (size_t)mBlocks) {
                rc = JUMP_DONE;
            }
        }
    }

    if (SUCCEED(rc)) {
        int32_t id = 0;
        for (auto iter : mQ) {
            if (iter.id != id++ ||
                ISNULL(iter.data)) {
                rc = JUMP_DONE;
            }
        }
    }

    return rc == NO_ERROR;
}

int32_t MemoryHolder::getType()
{
    return mType;
}


MemoryHolder::MemoryHolder(int32_t type, int32_t blocks, bool safe) :
    mType(type),
    mBlocks(blocks),
    mFlatten(NULL),
    mFlattenSize(0),
    mThreadSafe(safe),
    mModule(MODULE_UTILS)
{
    flush();
}

MemoryHolder::~MemoryHolder()
{
    flush();
    if (NOTNULL(mFlatten)) {
        SECURE_FREE(mFlatten);
    }
}

MemoryHolder::MemoryHolder(const MemoryHolder &rhs)
{
    *this = rhs;
}

MemoryHolder &MemoryHolder::operator=(const MemoryHolder &rhs)
{
    if (&rhs != this) {
        flush();
        for (auto iter : rhs.mQ) {
            int32_t rc = add(iter.id, iter.data, iter.size);
            if (!SUCCEED(rc)) {
                LOGE(mModule, "Failed to add id %d to self.", iter.id);
            }
        }
    }

    return *this;
}

int32_t MemoryHolder::getBlock(int32_t id, void **dat, int32_t *size)
{
    int32_t rc = NO_ERROR;

    for (auto iter : mQ) {
        if (id == iter.id) {
            *dat  = iter.data;
            *size = iter.size;
            break;
        }
    }

    return rc;
}

int8_t MemoryHolder::operator[](int32_t offset)
{
    int8_t result = 0;

    for (auto iter : mQ) {
        if (offset - iter.size <= 0) {
            int8_t *_data = (int8_t *)iter.data;
            result = _data[offset];
            break;
        } else {
            offset -= iter.size;
        }
    }

    return result;
}


int32_t MemoryHolder::setCb(
    std::function<int32_t (int32_t type, void *data, int32_t size)> cb)
{
    if (NOTNULL(cb)) {
        mCb = cb;
    }

    return NO_ERROR;
}

};


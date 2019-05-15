#include "MemMgmt.h"
#include "Pool.h"

namespace sirius {

MemMgmt defaultMemPool;

const size_t MemMgmt::mMaxTotalSize = MAX_TOTAL_ALLOC_SIZE;
const size_t MemMgmt::mMaxAllocSize = MAX_ALLOCABLE_SIZE;

inline void *CalcOffset(void *ptr, size_t offset) {
    return (void *)((char *)ptr + offset);
}

MemMgmt::MemMgmt(std::string name, bool enablePool) :
    mModule(MODULE_MEMORY_POOL), mName(name), mCnt(0),
    mId(0), mReserveLen(sizeof(size_t)),
    mTotalSize(0), mEnablePool(enablePool), mPool(NULL)
{
    if (mEnablePool) {
        mPool = CreateMemoryPool(1024);
        if (ISNULL(mPool)) {
            mEnablePool = false;
            LOGE(mModule, "Failed to create memory pool, ignore");
        }
    }
}

MemMgmt::~MemMgmt()
{
    mMgrLock.writeLock();
    if (mCnt > 0) {
        LOGE(mModule, "%d bytes in %u pieces of memory leak detected " \
            "in pool %s, force recycling", mCnt, mTotalSize, mName.c_str());
        mMgrLock.unlock();
        Dump();
        RWLock::AutoWLock l(mMgrLock);
        RWLock::AutoRLock ldb(mDBLock);
        for (std::list<MemInfo>::iterator iter = mMemDB.begin();
            iter != mMemDB.end(); iter++) {
            __free(CalcOffset(iter->data, -mReserveLen));
        }
    }

    if (mEnablePool && NOTNULL(mPool)) {
        DestroyMemoryPool(&mPool);
        mEnablePool = false;
    }
}

void *MemMgmt::_malloc(size_t size, const char *file,
    const char *func, const int32_t line)
{
    if (size == 0) {
        LOGE(mModule, "Malloc 0 byte request from %s %s %d",
            file, func, line);
        return NULL;
    }

    RWLock::AutoWLock l(mMgrLock);
    if (size > mMaxAllocSize) {
        LOGE(mModule, "Alloc size %u beyond pool limitation %u",
            size, mMaxAllocSize);
        return NULL;
    }
    if ((mTotalSize + size) > mMaxTotalSize) {
        LOGE(mModule, "Memory pool would over flow %u bytes %u:%u at %s %s %d",
            size, mTotalSize, mMaxTotalSize, file, func, line);
        return NULL;
    }

    void *dat = __malloc(size + 2 * mReserveLen);
    if (!dat) {
        LOGE(mModule, "Failed to alloc %u bytes for file %s func %s line %d",
            size, file, func, line);
    } else {
        mCnt++;
        mId++;
        mTotalSize += size;
        *(size_t *)dat = size;
        *(size_t *)CalcOffset(dat, size + mReserveLen) = size;
        MemInfo memInfo = {
            .id = mId,
            .data = CalcOffset(dat, mReserveLen),
            .size = size,
            .file = file,
            .func = func,
            .line = line,
        };
        RWLock::AutoWLock ldb(mDBLock);
        mMemDB.push_back(memInfo);
    }

    return CalcOffset(dat, mReserveLen);
}

void MemMgmt::_free(void *data, const char *file,
    const char *func, const int32_t line)
{
    if (data == NULL) {
        LOGE(mModule, "Fatal: Free NULL detected at %s %s %d",
            file, func, line);
        return;
    }

    RWLock::AutoWLock l(mMgrLock);
    bool found = false;
    std::list<MemInfo>::iterator iter;

    if (!found) {
        RWLock::AutoRLock ldb(mDBLock);
        for (iter = mMemDB.begin(); iter != mMemDB.end(); iter++) {
            if (iter->data == data) {
                found = true;
                break;
            }
        }
        if (!found) {
            LOGE(mModule, "Mem %p by %s %s %d not found in this pool, " \
                "freed twice or not allocated", data, file, func, line);
            return;
        }
    }

    if (found) {
        if (*(size_t *)CalcOffset(data, iter->size) != iter->size ||
            *(size_t *)CalcOffset(data, -mReserveLen) !=
                *(size_t *)CalcOffset(data, iter->size)) {
            LOGE(mModule, "Fatal: Memory %p len %u alloced by %s %s %d " \
                "freed by %s %s %d out of bounds",
                iter->data, iter->size,
                iter->file, iter->func, iter->line,
                file, func, line);
        }
        __free(CalcOffset(data, -mReserveLen));
        mTotalSize -= iter->size;
        mCnt--;
        RWLock::AutoWLock ldb(mDBLock);
        mMemDB.erase(iter);
    }
}

void MemMgmt::_dump(const char *file, const char *func, const int32_t line)
{
    LOGE(mModule, "Memory dump triggered by file %s func %s line %d",
        file, func, line);

    RWLock::AutoRLock l(mMgrLock);
    if (mCnt) {
        LOGE(mModule, "%u bytes in %d pieces memory remaining",
            mTotalSize, mCnt);
    } else {
        LOGE(mModule, "memory pool is empty, 0 bytes remaining");
    }

    RWLock::AutoRLock ldb(mDBLock);
    for (std::list<MemInfo>::iterator iter = mMemDB.begin();
        iter != mMemDB.end(); iter++) {
        LOGE(mModule, "Mem %d %p alloced by %s %s %d len %u",
            iter->id, iter->data,
            iter->file, iter->func, iter->line, iter->size);
        if (*(size_t *)CalcOffset(iter->data, iter->size) != iter->size ||
            *(size_t *)CalcOffset(iter->data, -mReserveLen) !=
                *(size_t *)CalcOffset(iter->data, iter->size)) {
            LOGE(mModule, "Fatal: Memory %d %p len %u alloced " \
                "by %s %s %d out of bounds",
                iter->id, iter->data, iter->size,
                iter->file, iter->func, iter->line);
        }
    }
}

void *MemMgmt::__malloc(size_t size)
{
    void *result = NULL;
    if (mEnablePool && NOTNULL(mPool)) {
        result = PoolMalloc(mPool, size);
    } else {
        result = malloc(size);
    }

    return result;
}

void MemMgmt::__free(void *ptr)
{
    if (mEnablePool && NOTNULL(mPool)) {
        PoolFree(mPool, ptr);
    } else {
        free(ptr);
    }
}

};


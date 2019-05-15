#ifndef _OBJECT_BUFFER_HPP_H_
#define _OBJECT_BUFFER_HPP_H_

#include "ObjectBuffer.h"
#include "logs.h"
#include "Atomic.h"

namespace sirius {

template<typename T>
ObjectBuffer<T>::ObjectInfo::ObjectInfo(T *aobj,
    const char *afile, const char *afunc, const int32_t aline) :
    obj(aobj),
    file(afile),
    func(afunc),
    line(aline)
{
}

template<typename T>
ObjectBuffer<T>::ObjectBuffer(uint32_t capacity) :
    mDebug(false),
    mCapacity(capacity),
    mCnt(0),
    mModule(MODULE_OBJECT_BUFFER)
{
    mDebug = true;
}

template<typename T>
ObjectBuffer<T>::~ObjectBuffer()
{
    flush();
}

template<typename T>
bool ObjectBuffer<T>::withinCapacity()
{
    return !mCapacity ? true :
        (mCnt < static_cast<int32_t>(mCapacity));
}

template<typename T>
int32_t ObjectBuffer<T>::flush()
{
    while (mFree.size() > 0) {
        T *obj = static_cast<T *>(mFree.dequeue());
        if (NOTNULL(obj)) {
            delete obj;
        }
    }

    mFree.flush();
    mFree.init();
    mFreeCnt = 0;

    return NO_ERROR;
}

template<typename T>
int32_t ObjectBuffer<T>::dump()
{
    LOGI(mModule, "Dump objects buffer:");
    LOGI(mModule, "  Maximum size of objects buffer: %d", mCapacity);
    LOGI(mModule, "  Current usage: %d free, %d taken", mFreeCnt, mCnt);

    RWLock::AutoRLock l(mTakenInfLock);
    if (mTakenInf.size() > 0) {
        uint32_t cnt = 0;
        LOGI(mModule, "    Here is list of taken objects:");
        for(auto &inf : mTakenInf) {
            LOGI(mModule, "    id:%3d, %d bytes, taken by %s | %s | +%d",
                ++cnt, sizeof(T), inf->file, inf->func, inf->line);
        }
    }

    LOGI(mModule, "Dump objects buffer done.");

    return NO_ERROR;
}

template<typename T>
T *ObjectBuffer<T>::get(BlockType block)
{
    T *result = static_cast<T *>(mFree.dequeue());

    if (!ISNULL(result)) {
        utils_atomic_dec(&mFreeCnt);
    } else if (withinCapacity()) {
        result = new T();
    } else if (block == NONBLOCK_TYPE) {
        result = NULL;
    } else {
        do {
            Semaphore *sem = new Semaphore();
            mSems.enqueue(sem);
            sem->wait();
            delete sem;

            result = static_cast<T *>(mFree.dequeue());
        } while (ISNULL(result));
        utils_atomic_dec(&mFreeCnt);
    }

    if (!ISNULL(result)) {
        utils_atomic_inc(&mCnt);
    }

    return result;
}

template<typename T>
int32_t ObjectBuffer<T>::put(T *obj)
{
    mFree.enqueue(obj);
    utils_atomic_inc(&mFreeCnt);
    utils_atomic_dec(&mCnt);

    Semaphore *sem =
        static_cast<Semaphore *>(mSems.dequeue());
    if (!ISNULL(sem)) {
        sem->signal();
    }

    return NO_ERROR;
}

template<typename T>
T *ObjectBuffer<T>::get(BlockType block,
    const char *file, const char *func, const int32_t line)
{
    T *obj = get(block);

    if (!ISNULL(obj)) {
        sp<ObjectInfo> inf = new ObjectInfo(obj, file, func, line);
        RWLock::AutoWLock l(mTakenInfLock);
        mTakenInf.push_back(inf);
    } else {
        LOGE(mModule, "Failed to get object from object buffer");
    }

    return obj;
}

template<typename T>
int32_t ObjectBuffer<T>::put(T *obj,
    const char *file, const char *func, const int32_t line)
{
    int32_t rc = NO_ERROR;
    bool found = false;

    {
        RWLock::AutoWLock l(mTakenInfLock);
        auto iter = mTakenInf.begin();
        while (iter != mTakenInf.end()) {
            if ((**iter).obj == obj) {
                iter = mTakenInf.erase(iter);
                found = true;
            } else {
                iter++;
            }
        }
    }

    if (!found) {
        LOGE(mModule, "Object %p not get from object buffer %s | %s | +%d",
            obj, file, func, line);
        rc = NO_ERROR;
    }

    put(obj);

    return rc;
}

template<typename T>
int32_t ObjectBuffer<T>::dump(const char *file,
    const char *func, const int32_t line)
{
    LOGI(mModule, "Dump by  %s | %s | +%d", file, func, line);
    return dump();
}

};

#endif

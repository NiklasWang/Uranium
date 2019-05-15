#ifndef _MEMORY_HOLDER_H_
#define _MEMORY_HOLDER_H_

#include <functional>
#include <list>

#include "modules.h"
#include "CQueue.h"
#include "RWLock.h"

namespace sirius {

#define NO_BLOCK_LIMIT  0
#define SAFE_HOLDER     true
#define NON_SAFE_HOLDER false
#define COPY_BUFFER     true
#define DO_NOT_COPY_BUF false

class MemoryHolder {
public:
    int32_t add(int32_t id, void *dat, int32_t size, bool copy);
    int32_t flatten(void **dat, int32_t *size);
    int32_t getBlock(int32_t id, void **dat, int32_t *size);
    int32_t flush();
    int32_t isCompleted();
    int32_t getType();
    int32_t setCb(std::function<int32_t (int32_t type, void *data, int32_t size)> cb);
    int8_t operator[](int32_t offset);

public:
    MemoryHolder(int32_t type,
        int32_t blocks = NO_BLOCK_LIMIT, bool safe = NON_SAFE_HOLDER);
    virtual ~MemoryHolder();
    MemoryHolder(const MemoryHolder &rhs);
    MemoryHolder &operator=(const MemoryHolder &rhs);

private:
    int32_t add(int32_t id, void *dat, int32_t size);

private:
    struct Block {
        int32_t id;
        int32_t size;
        void   *data;
    };

private:
    int32_t mType;
    int32_t mBlocks;
    std::list<Block> mQ;
    void      *mFlatten;
    int32_t    mFlattenSize;
    bool       mThreadSafe;
    RWLock     mLock;
    ModuleType mModule;
    std::function<int32_t (int32_t type, void *data, int32_t size)> mCb;
};

};

#endif

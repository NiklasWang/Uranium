#ifndef MEM_MGMT_H_
#define MEM_MGMT_H_

#include <string>
#include <list>

#include "common.h"
#include "RWLock.h"

namespace sirius {

#define MAX_TOTAL_ALLOC_SIZE (80 * 1024 * 1024)
#define MAX_ALLOCABLE_SIZE   (32 * 1024 * 1024 + 1)

struct MemoryPool;

class MemMgmt :
    virtual public noncopyable {

public:
    MemMgmt(std::string name = "defaultMemPool", bool pool = true);
    ~MemMgmt();
    void *_malloc(size_t size, const char *file = "",
        const char *func = "", const int32_t line = 0);
    void _free(void *data, const char *file = "",
        const char *func = "", const int32_t line = 0);
    void _dump(const char *file = "",
        const char *func = "", const int32_t line = 0);

public:
    struct MemInfo {
        int32_t       id;
        void         *data;
        size_t        size;
        const char   *file;
        const char   *func;
        const int32_t line;
    };

private:
    void *__malloc(size_t size);
    void __free(void *ptr);

private:
    ModuleType    mModule;
    RWLock        mMgrLock;
    std::string   mName;
    std::list<MemInfo> mMemDB;
    RWLock        mDBLock;
    int32_t       mCnt;
    int32_t       mId;
    size_t        mReserveLen;
    size_t        mTotalSize;
    bool          mEnablePool;
    MemoryPool   *mPool;
    static const size_t mMaxTotalSize;
    static const size_t mMaxAllocSize;
};

#define DeclareMemPool(pool)   (pool) = new MemMgmt("MemPool")
#define DeleteMemPool(pool)    do { delete (pool); (pool) = NULL; } while(0)
#define MallocFrom(pool, size) (pool)._malloc((size), __FILE__, __FUNCTION__, __LINE__)
#define FreeFrom(pool, ptr)    (pool)._free((ptr), __FILE__, __FUNCTION__, __LINE__)
#define DumpFrom(pool)         (pool)._dump(__FILE__, __FUNCTION__, __LINE__)

#ifdef MEMORY_POOL_MODE
extern MemMgmt defaultMemPool;

#define Malloc(size)  defaultMemPool._malloc((size), __FILE__, __FUNCTION__, __LINE__)
#define Free(ptr)     defaultMemPool._free((ptr), __FILE__, __FUNCTION__, __LINE__)
#define Dump()        defaultMemPool._dump(__FILE__, __FUNCTION__, __LINE__)
#else
#define Malloc(size)  malloc((size))
#define Free(ptr)     free((ptr))
#define Dump()
#endif

};

#endif

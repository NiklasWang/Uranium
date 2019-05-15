#ifndef __POOL_H_
#define __POOL_H_

namespace sirius {

struct MemoryPool;

/**
 * @brief Create memory pool, so can allocate memory after that.
 *
 * It doesn't means can't get memory if bigger than given size, pool will deliver to system functions,
 * so that performance is equal as system. When destroy pool, make sure you have release all memory block
 * smaller than [uMaxStrLen], pool doesn't track them, if bigger than [uMaxStrLen], pool will release all
 * of them to prevent memory leak.
 *
 * @param uMaxStrLen Max length of string this pool can allocate.
 * @return Created memory pool.
 */
MemoryPool *CreateMemoryPool(unsigned int maxLen);

/**
 * @brief Destroy memory pool, release all idle memory block smaller than max size pool can allocate, and
 * release all blocks bigger than this size not matter using or not.
 *
 * @param pPool Which pool to destroy, set to NULL when finished to destroy.
 */
void DestroyMemoryPool(MemoryPool **pool);

/**
 * @biref Get a memory block from pool.
 *
 * @param pPool From which pool to get.
 * @param uSize Size of string want to allocate.
 * @return Allocated memory.
 */
void *PoolMalloc(MemoryPool *pool, unsigned int size);

/**
 * @brief Back a memory block to pool so that it can be use again.
 *
 * @param pPool Back to which pool.
 * @param pPtr Address of memory block to back.
 */
void PoolFree(MemoryPool *pool, void *ptr);

};

#endif

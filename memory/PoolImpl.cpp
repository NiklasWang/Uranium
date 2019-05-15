/**
 * @file   PoolImpl.cpp
 *
 * @date   Oct 20, 2011
 * @author WangLiang
 * @email  WangLiangCN@live.com
 *
 * @brief  Variable length, Able to recycle, List style memory pool.
 */

#include "PoolImpl.h"
#include "logs.h"

#define PrintWarning(fmt, args...) LOGI(MODULE_MEMORY_POOL, fmt, ##args)
#define PrintError(fmt, args...)   LOGE(MODULE_MEMORY_POOL, fmt, ##args)

namespace sirius {

/**
 * @brief Data length type, should aligned with CPU arch.
 *
 * ARM will align with 4 bytes, so size type should have times of 4 bytes.
 */
typedef unsigned int sizetype;

/**
 * @brief Align function, convert it to aligned size.
 */
inline sizetype RoundUp(sizetype size) {
    return (((size) + (ALIGN_SIZE - 1)) & ~(ALIGN_SIZE - 1));
}

/**
 * @brief Get index from block table by given size.
 */
inline sizetype GetIndex(sizetype size) {
    return (((size) + (ALIGN_SIZE - 1)) / (ALIGN_SIZE) - 1);
}

/**
 * @brief Get offset ponter of base pointer.
 */
inline void *GetOffset(void *ptr, off_t offset) {
    return (void *)((char *)ptr + offset);
}

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
MemoryPool *CreateMemoryPool(unsigned int uMaxStrLen)
{
    uMaxStrLen = (uMaxStrLen > MAX_STRING_LEN) ? MAX_STRING_LEN : uMaxStrLen;
    sizetype uFreeTableLen = GetIndex(uMaxStrLen) + 1;

    size_t uAllocSize = sizeof(MemoryPool) + (sizeof(Head) * uFreeTableLen);
    MemoryPool *pPool = (MemoryPool *)malloc(uAllocSize);
    if (NULL != pPool) {
        pPool->uMaxSize = uMaxStrLen;
        pPool->pFirstBigBlock = NULL;
        pPool->pTable = (BlockTable_t *)GetOffset(pPool, sizeof(MemoryPool));
        for (sizetype i = 0; i < uFreeTableLen; ++i) {
            pPool->pTable[i].pFirstNode = NULL;
            pPool->pTable[i].uIdleNum = 0;
        }
    } else {
        PrintError("%s: Failed to alloc %u bytes to create pool",
            __FUNCTION__, uAllocSize);
    }

    return pPool;
}

/**
 * @brief Destroy memory pool, release all idle memory block smaller than max size pool can allocate, and
 * release all blocks bigger than this size not matter using or not.
 *
 * @param pPool Which pool to destroy, set to NULL when finished to destroy.
 */
void DestroyMemoryPool(MemoryPool **pPool)
{
    assert(NULL != pPool);
    assert(NULL != *pPool);
    Node *pCurrNode = NULL;
    Node *pPreNode = NULL;
    BigBlock *pCurrBlock = NULL;
    BigBlock *pPreBlock = NULL;

    // Release idle blocks in pool.
    sizetype uFreeTableLen = GetIndex((*pPool)->uMaxSize) + 1;
    for (sizetype i = 0; i < uFreeTableLen; ++i) {
        pCurrNode = (*pPool)->pTable[i].pFirstNode;
        while(NULL != pCurrNode) {
            pPreNode = pCurrNode;
            pCurrNode = pCurrNode->pNext;
            free(pPreNode);
        }
    }

    // Release block which bigger than pool can allocate.
    pCurrBlock = (*pPool)->pFirstBigBlock;
    while(NULL != pCurrBlock) {
        pPreBlock = pCurrBlock;
        pCurrBlock = pCurrBlock->pNext;
        free(pPreBlock);
    }

    // Release pool.
    free(*pPool);
    *pPool = NULL;
}

/**
 * @biref Get a memory block from pool.
 *
 * @param pPool From which pool to get.
 * @param uSize Size of string want to allocate.
 * @return Allocated memory.
 */
void *PoolMalloc(MemoryPool *pPool, unsigned int uSize)
{
    assert(NULL != pPool);
    assert(0 != uSize);
    sizetype uIndex = GetIndex(uSize);
    void *pPtr = NULL;
    void *pResult = NULL;

    // If user want to allocate a memory bigger than pool can do,
    // Deliver this to system and record it.
    if (uSize > pPool->uMaxSize) {
        pPtr = malloc(sizeof(BigBlock) + sizeof(sizetype) + uSize);
        BigBlock *pBigBlock = (BigBlock *)pPtr;
        if (NULL != pPtr) {
            *((sizetype *)GetOffset(pPtr, sizeof(BigBlock))) = uSize;
            pBigBlock->data = GetOffset(pPtr, sizeof(BigBlock) + sizeof(sizetype));
            pBigBlock->pPre = NULL;
            pBigBlock->pNext = NULL;
            if (pPool->pFirstBigBlock != NULL) {
                pBigBlock->pNext = pPool->pFirstBigBlock;
                pPool->pFirstBigBlock->pPre = pBigBlock;
            }
            pPool->pFirstBigBlock = pBigBlock;
        }
        pResult = (NULL != pPtr) ? pBigBlock->data : NULL;
    } else {
        // Check if there are idle blocks can be use again, or allocate new blocks from system.
        if (NULL != (pPool->pTable[uIndex].pFirstNode)) {
            pPtr = (void *)&(pPool->pTable[uIndex].pFirstNode->data);
            pPool->pTable[uIndex].pFirstNode = pPool->pTable[uIndex].pFirstNode->pNext;
            -- (pPool->pTable[uIndex].uIdleNum);
            *(sizetype *)pPtr = uSize;
            pResult = GetOffset(pPtr, sizeof(sizetype));
        } else {
            sizetype uLen = sizeof(sizetype) + RoundUp(uSize);
            uLen = (uLen > sizeof(Node)) ? uLen : sizeof(Node);
            pPtr = malloc(uLen);
            if (NULL != pPtr) {
                *(sizetype *)pPtr = uSize;
                pResult = GetOffset(pPtr, sizeof(sizetype));
            } else {
                pResult = NULL;
            }
        }
    }

    if (NULL == pResult) {
        PrintError("%s: Failed to malloc %d bytes in pool", __FUNCTION__, uSize);
    }

    return pResult;
}

/**
 * @brief Back a memory block to pool so that it can be use again.
 *
 * @param pPool Back to which pool.
 * @param pPtr Address of memory block to back.
 */
void PoolFree(MemoryPool *pPool, void *pPtr)
{
    if (NULL == pPool) {
        PrintWarning("%s : Ptr %p not belongs to pool, release directly",
            __FUNCTION__, pPtr);
        free(pPtr);
        return;
    }

    // Check if big blocks allocated from system directly,
    // If so, release it to system, pool won't use it.
    pPtr = GetOffset(pPtr, -sizeof(sizetype));
    sizetype uSize = *((sizetype *)pPtr);
    if (uSize > pPool->uMaxSize) {
        pPtr = GetOffset(pPtr, -sizeof(BigBlock));
        BigBlock *pBigBlock = (BigBlock *)pPtr;
        if (NULL == pBigBlock->pPre) {
            pPool->pFirstBigBlock = pBigBlock->pNext;
            if (pPool->pFirstBigBlock) {
                pPool->pFirstBigBlock->pPre = NULL;
            }
        } else if(NULL == pBigBlock->pNext){
            pBigBlock->pPre->pNext = NULL;
        } else {
            pBigBlock->pPre->pNext = pBigBlock->pNext;
            pBigBlock->pNext->pPre = pBigBlock->pPre;
        }
        free(pPtr);
    } else {
        sizetype uIndex = GetIndex(uSize);
        if ((pPool->pTable[uIndex].uIdleNum) < RECYCLE_IF_MORETHAN_BLOCKS) {
            // Back the memory block to pool so that can use it again.
            Node *pNode = (Node *)pPtr;
            pNode->pNext = pPool->pTable[uIndex].pFirstNode;
            pPool->pTable[uIndex].pFirstNode = pNode;
            ++ (pPool->pTable[uIndex].uIdleNum);
        } else {
            // Release to system since too many idle blocks in pool.
            free(pPtr);
        }
    }

    return;
}

};


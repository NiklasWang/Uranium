/**
 * @file   PoolImpl.h
 *
 * @date   Oct 20, 2011
 * @author WangLiang
 * @email  WangLiangCN@live.com
 *
 * @brief  Variable length, Able to recycle, List style memory pool.
 *
 * Data structure:
 *
 * MemoryPool_t             0      align   2*align   3*align   4*align   5*align           (n-1)*align
 * +--------+          ~align   ~2*align  ~3*align  ~4*align  ~5*align  ~6*align    ...    ~n*align
 * |        |       +---------+---------+---------+---------+---------+---------+---------+-----------+
 * | pTable |  -->  |   Head  |   Head  |   Head  |   Head  |   Head  |   Head  |   ...   |    Head   |
 * |        |       +---------+---------+---------+---------+---------+---------+---------+-----------+
 * |        |           |          |         |         |         |         |                    |
 * +--------+        +------+  +------+   +------+  +------+   NULL      NULL               +------+
 * |        |        | Not  |  | Not  |   | Not  |  | Not  |                                | Not  |
 * |  uMax  |        | Used |  | Used |   | Used |  | Used |                                | Used |
 * |  Size  |        +------+  +------+   +------+  +------+                                +------+
 * |        |           |          |         |         |                                        |
 * +--------+        +------+    NULL      NULL                                               NULL
 * |        |        | Not  |                         ...
 * |  pBig  |        | Used |
 * |  Block |----    +------+                          |
 * |        |   |       |                           +------+
 * +--------+   |     NULL                          | Not  |
 *              |                                   | Used |
 *              |                                   +------+
 *              |                                      |
 *              |                                     NULL
 *              |
 *              |
 *              |     +-------+    +-------+    +-------+              +-------+
 *              ----> | Using | -- | Using | -- | Using | --  ...  --  | Using |  --  NULL
 *                    +-------+    +-------+    +-------+              +-------+
 */

#ifndef MEMORYPOOL_H_
#define MEMORYPOOL_H_

#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>

namespace sirius {

/**
 * @brief Max length of string pool can allocate, if bigger than this when creating, down to this size.
 */
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif
#define MAX_STRING_LEN USHRT_MAX

/**
 * @brief Align size, must be 2^n
 */
#define ALIGN_SIZE 8

/**
 * @brief Maximum number of idle block in memory pool, if more than these, release them.
 */
#define RECYCLE_IF_MORETHAN_BLOCKS 16

/**
 * @brief Allocate size of memory, if it not used, let the first four block save the pointer pointed to
 * next free allocated memory, if this memory are using, [data] is the first address of this memory.
 */
union Node {
    union Node *pNext;    ///< If this memory is idle, this pointed to next free memory block.
    char data[1];         ///< If this memory is using, this is the address of this block.
};

/**
 * @brief Head of list of each idle memory block. If more than a given number of idle blocks, release them.
 */
struct Head {
    Node *pFirstNode;      ///< First idle memory block.
    unsigned int uIdleNum;   ///< Number of idle memory blocks in list.
};

/**
 * @brief Block table is an array, each elements pointed to a list which describes the free memory block.
 */
typedef Head BlockTable_t;

/**
 * @brief Memory pool have it's biggest size, if user asked to allocate a big block bigger than memory
 * can be, pool will deliver this command to system and record this, when destroy the pool, all allocated
 * big block will be released.
 */
struct BigBlock {
    void *data;               ///< Start address of allocated big block.
    struct BigBlock *pNext;   ///< Next allocated big block if this not the last one.
    struct BigBlock *pPre;    ///< Previous allocated big block if that exists.
};

/**
 * @biref Information about memory pool.
 */
struct MemoryPool {
    unsigned int uMaxSize;       ///< Longest block memory pool can allocate, if bigger, deliver to system.
    BlockTable_t *pTable;        ///< An array, each pointed to a list which describes the free memory block.
    BigBlock *pFirstBigBlock;  ///< If bigger than pool can allocate, pointed to list which contains them.
};

};

#endif /* MEMORYPOOL_H_ */

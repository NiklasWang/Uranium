/**
 * @file   Tester.c
 *
 * @date   Oct 25, 2011
 * @author WangLiang
 * @email  WangLiangCN@live.com
 *
 * @brief  Test program for VALMemoryPool.
 */

#include "PoolImpl.h"
#include "Pool.h"
#include <time.h>
#include <sys/time.h>

namespace sirius {

/**
 * @brief Longest length of string can allocate from pool.
 */
#define MALLOC_MAX_LEN (1*1024)

/**
 * @brief Allocate so many times from memory pool in a turn.
 */
#define TEST_MALLOC_TIMES 9999

/**
 * @brief Test for so many turns in one test.
 */
#define TEST_RETRY_TIMES 99

/**
 * @brief Make a random string.
 *
 * @param str Where to save generated string.
 * @param strLen Length of string want to generate.
 */
inline void GenerateRandStr(char *str, int strLen)
{
    srand(time(NULL) + rand());
    int i = 0;
    for (; i  < strLen; i++) {
        str[i] = rand() %94 + 32;
    }
    str[i] = '\0';
}

#define Malloc(pool, size) PoolMalloc((pool), (size))
#define Free(pool, ptr)    PoolFree((pool), (ptr))

/**
 * @brief Tester for VALMemoryPool.
 */
int VALMemoryPoolTester()
{
    // To compute used time.
    struct timeval startTime, endTime;
    unsigned long long costTime = 0ULL;

    // To store available memory address got from system or pool.
    char **pStrings = (char **)malloc(sizeof(char *) * TEST_MALLOC_TIMES);
    // Generate variable length of string.
    int aStrLen[TEST_MALLOC_TIMES];
    srand((unsigned int)time(NULL) + rand());
    for (int i=0; i<TEST_MALLOC_TIMES; ++i) {
        aStrLen[i] = rand() % MALLOC_MAX_LEN;
        srand(aStrLen[i]);
    }

    // Memory pool Malloc/Free test.
    printf("Now testing memory pool Malloc/Free, VAL memory pool.");
    gettimeofday(&startTime, NULL);

    MemoryPool *pPool = CreateMemoryPool(MALLOC_MAX_LEN);
    for (int i = 0; i < TEST_RETRY_TIMES; ++i) {
        for (int j = 0; j < TEST_MALLOC_TIMES; ++j) {
            pStrings[j] = (char *)Malloc(pPool, sizeof(char) * (aStrLen[j] + 1));
            //GenerateRandStr(pStrings[j], aStrLen[j]);
            *pStrings[j] = '\0';
            Free(pPool, pStrings[j]);
        }
    }
    /*
     * The following code tests when wants to allocate a big block memory than pool can do, then deliver
     * action to system, and record this memory, when destroy memory pool, all allocated big block will
     * be release.
     */
    /*for (int i=0; i<TEST_MALLOC_TIMES; ++i)
    {
        pStrings[i] = Malloc(pPool, sizeof(char) * (aStrLen[i] + MALLOC_MAX_LEN));
    }*/
    DestroyMemoryPool(&pPool);

    gettimeofday(&endTime, NULL);
    costTime = 1000 * 1000 * (endTime.tv_sec - startTime.tv_sec) + endTime.tv_usec - startTime.tv_usec;
    printf("Memory pool Malloc/Free tested, malloc and free %d strings for %d times, cost %llu us.\n",
            TEST_MALLOC_TIMES, TEST_RETRY_TIMES, costTime);

    free(pStrings);
    return 0;
}

/**
 * @brief Default system allocator's time counter, allocate variable length of string every time.
 */
int SystemDefaultAllocatorTest_VarLen()
{
    // To compute used time.
    struct timeval startTime, endTime;
    unsigned long long costTime = 0ULL;

    // To store available memory address got from system or pool.
    char **pStrings = (char **)malloc(sizeof(char *) * TEST_MALLOC_TIMES);
    // Generate variable length of string.
    int aStrLen[TEST_MALLOC_TIMES];
    srand((unsigned int)time(NULL) + rand());
    for (int i = 0; i < TEST_MALLOC_TIMES; ++i) {
        aStrLen[i] = rand() % MALLOC_MAX_LEN + 1;
        srand(aStrLen[i]);
    }

    // System default malloc/free test.
    printf("Now testing system default malloc/free, variable length.");
    gettimeofday(&startTime, NULL);

    for (int i = 0; i < TEST_RETRY_TIMES; ++i) {
        for (int j = 0; j < TEST_MALLOC_TIMES; ++j) {
            pStrings[j] = (char *)malloc(sizeof(char)*(aStrLen[j] + 1));
            //GenerateRandStr(pStrings[j], aStrLen[j]);
            *pStrings[j] = '\0';
            free(pStrings[j]);
        }
    }

    gettimeofday(&endTime, NULL);
    costTime = 1000 * 1000 * (endTime.tv_sec - startTime.tv_sec) + endTime.tv_usec - startTime.tv_usec;
    printf("System default malloc/free tested, malloc and free %d strings for %d times, cost %llu us.\n",
            TEST_MALLOC_TIMES, TEST_RETRY_TIMES, costTime);

    free(pStrings);
    return 0;
}

int __main()
{
    SystemDefaultAllocatorTest_VarLen();
    return VALMemoryPoolTester();
}

};


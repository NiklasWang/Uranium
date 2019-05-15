#ifndef _SIRIUS_COMMON_H_
#define _SIRIUS_COMMON_H_

#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "version.h"
#include "modules.h"
#include "logs.h"
#include "SiriusServerIntf.h"

namespace sirius {

enum err_raeson {
    NO_ERROR = 0,
    PERM_DENIED,
    NOT_READY,
    PARAM_INVALID,
    INVALID_FORMAT,
    NO_MEMORY,
    SYS_ERROR,
    TIMEDOUT,
    NOT_FOUND,
    NOT_EXIST,
    ALREADY_INITED,
    ALREADY_EXISTS,
    NOT_INITED,
    NOT_REQUIRED,
    NOT_SUPPORTED,
    BAD_PROTOCAL,
    USER_ABORTED,
    JUMP_DONE,
    TEST_FAILED,
    UNKNOWN_ERROR,
};

#define SUCCEED(rc)      ((rc) == NO_ERROR)
#define ISNULL(ptr)      ((ptr) == NULL)
#define FAILED(rc)       (!SUCCEED(rc))
#define NOTNULL(ptr)     (!ISNULL(ptr))
#define POSITIVE_NUM(num) ((num) > 0)
#define NEGITIVE_NUM(num) ((num) < 0)
#define ISZERO(num)       ((num) == 0)
#define ARRAYSIZE(array) (sizeof(array) / sizeof(array[0]))
#define INVALIDNUM       (-1)
#define EQUALPTR(a, b)   ((a) == (b))
#define MAX(a, b)        ((a) > (b) ? (a) : (b))
#define MIN(a, b)        ((a) < (b) ? (a) : (b))
#define RESETRESULT(rc)  ((rc) = NO_ERROR)
#define RETURNIGNORE(rc, ignore)  ((rc) & (~(ignore)))
#define SUCCEEDIGNORE(rc, ignore) (SUCCEED(rc) || ((rc) == (ignore)))
#define EPSINON          1e-7
#define EQUALFLOAT(a, b) (fabsf((a) - (b)) < EPSINON)

#define SECURE_FREE(ptr) \
    do { \
        if (!ISNULL(ptr)) { \
            Free(ptr); \
            (ptr) = NULL; \
        } \
    } while(0)

#define SECURE_DELETE(obj) \
    do { \
        if (!ISNULL(obj)) { \
            delete(obj); \
            (obj) = NULL; \
        } \
    } while(0)

#define COMPARE_SAME_STRING(LHS, RHS) \
    ({ \
        bool _result = true; \
        if (NOTNULL(LHS) && NOTNULL(RHS)) { \
            _result &= !strcmp(LHS, RHS); \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else { \
            _result = false; \
        } \
        _result; \
    })

#define COMPARE_SAME_LEN_STRING(LHS, RHS, len) \
    ({ \
        bool _result = true; \
        if (NOTNULL(LHS) && NOTNULL(RHS)) { \
            _result &= !strncmp(LHS, RHS, len); \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else { \
            _result = false; \
        } \
        _result; \
    })

#define COMPARE_SAME_DATA(LHS, RHS, SIZE) \
    ({ \
        bool _result = true; \
        if (NOTNULL(LHS) && NOTNULL(RHS)) { \
            _result &= !memcmp(LHS, RHS, SIZE); \
        } else if (ISNULL(LHS) && ISNULL(RHS)) { \
        } else { \
            _result = false; \
        } \
        _result; \
    })

#ifndef offsetof
#define offsetof(TYPE, MEMBER) \
    ((int32_t) &((TYPE *)0)->MEMBER)
#endif

#define container_of(ptr, type, member) ({                 \
    const typeof(((type *)0)->member) *__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })

#define align_len_to_size(len, size) ({                    \
        (((len) + (size) - 1) & ~((size) - 1)); });

class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}

private:
    noncopyable(const noncopyable&) = delete;
    const noncopyable& operator=(const noncopyable&) = delete;
};

template <typename T>
struct AnyType {
    // No real bussiness to do
    AnyType() = default;
    ~AnyType() = default;
    AnyType &operator=(const AnyType &rhs) = default;
    AnyType(const T &rhs) { *this = rhs; }
    AnyType &operator=(const T &) { return *this; }
};

};

#endif

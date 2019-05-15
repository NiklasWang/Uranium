#ifndef _THREAD_INTF_H_
#define _THREAD_INTF_H_

#include <stdint.h>

namespace sirius {

class ThreadIntf {
public:
    virtual int32_t processTask(void *dat) = 0;
    virtual int32_t taskDone(void *dat, int32_t rc) = 0;

public:
    virtual ~ThreadIntf() = default;
};

};

#endif

#include "Thread.h"

namespace sirius {

int32_t Thread::callFunc(TaskBase *task, bool release)
{
    int32_t rc = NO_ERROR;

    if (task->module == MODULE_THREAD_POOL) {
        TaskInf<InternalTask> *internal =
            static_cast<TaskInf<InternalTask> *>(task);
        rc = internal->func(internal->arg);
        if (release && NOTNULL(internal) && NOTNULL(internal->arg)) {
            SECURE_DELETE(internal->arg);
        }
    }

    return rc;
}

}

#include "Thread.h"
#include "SiriusServerImpl.h"

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

    if (task->module == MODULE_SIRIUS_IMPL) {
        TaskInf<SiriusServerImpl::TaskBase> *sirius =
            static_cast<TaskInf<SiriusServerImpl::TaskBase> *>(task);
        rc = sirius->func(sirius->arg);
        if (release && NOTNULL(sirius) && NOTNULL(sirius->arg)) {
            SECURE_DELETE(sirius->arg);
        }
    }

    return rc;
}

}

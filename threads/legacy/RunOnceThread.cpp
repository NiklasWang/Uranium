#include "RunOnceThread.h"

namespace sirius {

int32_t RunOnceThread::run(RunOnceFunc *func, void *in, void *out)
{
    int32_t rc = NO_ERROR;

    if (mRuning) {
        LOGE(mModule, "Already runing");
        rc = ALREADY_EXISTS;
    } else {
        mRuning = true;
    }

    if (SUCCEED(rc)) {
        rc = constructThread();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to construct run once thread");
        }
    }

    if (SUCCEED(rc)) {
        mFunc = func;
        mIn   = in;
        mOut  = out;
        rc = newTask(NULL);
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to start thread");
        }
    }

    if (!SUCCEED(rc)) {
        mRuning = false;
    }

    return rc;
}

bool RunOnceThread::isRuning()
{
    return mRuning;
}

int32_t RunOnceThread::exit()
{
    int32_t rc = NO_ERROR;

    if (mExited) {
        LOGE(mModule, "Already exited.")
        rc = ALREADY_EXISTS;
    } else {
        mExited = true;
        mRuning = false;
    }

    if (SUCCEED(rc)) {
        rc = mFunc->abortOnceFunc();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to abort run once func, %d", rc);
        }
    }

    if (SUCCEED(rc)) {
        mSync.wait();
    }

    if (SUCCEED(rc)) {
        rc = mRc;
    }

    if (SUCCEED(rc)) {
        rc = destructThread();
        if (!SUCCEED(rc)) {
            LOGE(mModule, "Failed to destruct run once thread");
        }
    }

    return rc;
}

int32_t RunOnceThread::processTask(void * /*dat*/)
{
    int32_t rc = NO_ERROR;

    if (NOTNULL(mFunc)) {
        rc = mFunc->runOnceFunc(mIn, mOut);
    }

    return rc;
}

int32_t RunOnceThread::taskDone(void * /*dat*/, int32_t rc)
{
    if (rc == USER_ABORTED) {
        rc = NO_ERROR;
    }

    mRc = rc;
    mSync.signal();

    if (NOTNULL(mFunc)) {
        mFunc->onOnceFuncFinished(rc);
    }

    mRuning = false;

    return NO_ERROR;
}

RunOnceThread::RunOnceThread() :
    mRuning(false),
    mExited(false),
    mSync(SYNC_TYPE),
    mModule(MODULE_RUN_ONCE_THREAD),
    mRc(NO_ERROR)
{
}

RunOnceThread::~RunOnceThread()
{
    if (!mExited) {
        exit();
    }
}

};


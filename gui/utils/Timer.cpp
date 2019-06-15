#include <QDateTime>
#include <QDebug>

#include "common.h"
#include "Times.h"
#include "Timer.h"

#define DEBUG_MODE_ON 0

namespace uranium {

Timer::Timer(QString name) :
    mName(name),
    mExit(false),
    mDebug(!!(DEBUG_MODE_ON)),
    mTimedSem(nullptr),
    mMs(0)
{
}

Timer::~Timer()
{
    if (!mExit) {
        exit();
    }
}

int32_t Timer::startNow(Ms ms, std::function<int32_t ()> func)
{
    int32_t rc = NO_ERROR;

    if (SUCCEED(rc)) {
        if (ms() <= 0) {
            qDebug() << "Invalid ms" << ms();
            rc = PARAM_INVALID;
        }
    }

    if (SUCCEED(rc)) {
        mMs = ms;
        mFunc = func;
        if (NOTNULL(mTimedSem)) {
            if (ms != mTimedSem->time()) {
                mTimedSem->signal();
                SECURE_DELETE(mTimedSem);
            }
        }
    }

    if (SUCCEED(rc)) {
        if (ISNULL(mTimedSem)) {
            mTimedSem = new TimedSemaphore(ms());
            if (ISNULL(mTimedSem)) {
                qDebug() << "Failed to create semaphore timed.";
                rc = NO_MEMORY;
            }
        }
    }

    if (SUCCEED(rc)) {
        if (!isRunning()) {
            start();
        }
        mSem.signal();
    }

    return NO_ERROR;
}

int32_t Timer::startNow(Sec sec, std::function<int32_t ()> func)
{
    return startNow(Ms(sec() * 1000), func);
}

int32_t Timer::stopNow()
{
    if (NOTNULL(mTimedSem)) {
        mTimedSem->signal();
    }

    return NO_ERROR;
}

int32_t Timer::exitNow()
{
    mExit = true;
    if (NOTNULL(mTimedSem)) {
        mTimedSem->signal();
    }
    mSem.signal();
    quit();
    wait();

    return NO_ERROR;
}

void Timer::run()
{
    do {
        if (mExit) {
            break;
        }

        mSem.wait();
        if (mExit) {
            break;
        } else {
            runTimer();
        }
    } while (!mExit);
}

int32_t Timer::runTimer()
{
    int32_t rc = NO_ERROR;

    if (!mExit) {
        if (mMs != 0 && NOTNULL(mTimedSem)) {
            if (mDebug) {
                QDateTime dataTime = QDateTime::currentDateTime();
                QString time = dataTime.toString("hh:mm:ss.zzz");
                qDebug() << "Timer debug, start wait for "
                          << mMs() << "ms, "
                          << "current time is " << time;
            }
            rc = mTimedSem->wait();
            if (mDebug) {
                QDateTime dataTime = QDateTime::currentDateTime();
                QString time = dataTime.toString("hh:mm:ss.zzz");
                qDebug() << "Timer debug, finished wait for "
                          << mMs() << "ms, " << rc << ", "
                          << "current time is " << time;
            }
        }
    }

    if (!SUCCEED(rc)) {
        SECURE_DELETE(mTimedSem);
        mTimedSem = nullptr;
        mFunc();
    }

    return rc;
}

}

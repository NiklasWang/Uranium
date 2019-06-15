#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include <QThread>
#include <QString>

#include "Times.h"
#include "Semaphore.h"
#include "TimedSemaphore.h"

namespace uranium {

class Timer :
    public QThread
{
public:
    int32_t startNow(Ms ms, std::function<int32_t ()> func);
    int32_t startNow(Sec sec, std::function<int32_t ()> func);
    int32_t stopNow();
    int32_t exitNow();

public:
    Timer(QString name = "default");
    virtual ~Timer();

protected:
    void run() override;

private:
    int32_t runTimer();

private:
    QString mName;
    bool    mExit;
    bool    mDebug;
    Semaphore mSem;
    TimedSemaphore *mTimedSem;
    Ms mMs;
    std::function<int32_t ()> mFunc;
};

}

#endif

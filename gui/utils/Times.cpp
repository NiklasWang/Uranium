#include "Times.h"

namespace uranium {

int64_t currentUs()
{
    int64_t t = 0;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    t = (int64_t)tv.tv_usec + tv.tv_sec * 1000000ll;

    return t;
}

void delayMs(int32_t ms)
{
    if (ms > 0) {
        Sleep(ms);
    }
}

void delaySec(int32_t sec)
{
    delayMs(sec * 1000);
}

Ms::Ms(int32_t ms) :
    mMs(ms)
{
}

bool Ms::operator==(const Ms &rhs)
{
    return mMs == rhs.mMs;
}

bool Ms::operator!=(const Ms &rhs)
{
    return !(*this == rhs);
}

Ms &Ms::operator=(const Ms &rhs)
{
    if (&rhs != this) {
        mMs = rhs.mMs;
    }

    return *this;
}

int32_t Ms::operator()()
{
    return mMs;
}

Sec::Sec(int32_t sec) :
    mSec(sec)
{
}

bool Sec::operator==(const Sec &rhs)
{
    return mSec == rhs.mSec;
}

bool Sec::operator!=(const Sec &rhs)
{
    return *this != rhs;
}

Sec &Sec::operator=(const Sec &rhs)
{
    if (&rhs != this) {
        mSec = rhs.mSec;
    }

    return *this;
}

int32_t Sec::operator()()
{
    return mSec;
}

LARGE_INTEGER getFILETIMEoffset()
{
    SYSTEMTIME s;
    FILETIME f;
    LARGE_INTEGER t;

    s.wYear = 1970;
    s.wMonth = 1;
    s.wDay = 1;
    s.wHour = 0;
    s.wMinute = 0;
    s.wSecond = 0;
    s.wMilliseconds = 0;
    SystemTimeToFileTime(&s, &f);
    t.QuadPart = f.dwHighDateTime;
    t.QuadPart <<= 32;
    t.QuadPart |= f.dwLowDateTime;
    return (t);
}

int32_t clock_gettime(int, struct timeval *tv)
{
    LARGE_INTEGER t;
    FILETIME      f;
    double               microseconds;
    static LARGE_INTEGER offset;
    static double        frequencyToMicroseconds;
    static int           initialized = 0;
    static BOOL          usePerformanceCounter = 0;

    if (!initialized) {
        LARGE_INTEGER performanceFrequency;
        initialized = 1;
        usePerformanceCounter = QueryPerformanceFrequency(&performanceFrequency);
        if (usePerformanceCounter) {
            QueryPerformanceCounter(&offset);
            frequencyToMicroseconds = (double)performanceFrequency.QuadPart / 1000000.;
        } else {
            offset = getFILETIMEoffset();
            frequencyToMicroseconds = 10.;
        }
    }

    if (usePerformanceCounter) {
        QueryPerformanceCounter(&t);
    } else {
        GetSystemTimeAsFileTime(&f);
        t.QuadPart = f.dwHighDateTime;
        t.QuadPart <<= 32;
        t.QuadPart |= f.dwLowDateTime;
    }

    t.QuadPart -= offset.QuadPart;
    microseconds = (double)t.QuadPart / frequencyToMicroseconds;
    t.QuadPart = microseconds;
    tv->tv_sec = t.QuadPart / 1000000;
    tv->tv_usec = t.QuadPart % 1000000;

    return 0;
}

}

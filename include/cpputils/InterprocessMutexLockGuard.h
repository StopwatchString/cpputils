#ifndef CPPUTILS_INTERPROCESS_MUTEX_LOCK_GUARD_H
#define CPPUTILS_INTERPROCESS_MUTEX_LOCK_GUARD_H

#include "cpputils\InterprocessMutex.h"

class InterprocessMutexLockGuard
{
public:
    InterprocessMutexLockGuard(InterprocessMutex& mutex)
        : mutex(mutex)
    {
        mutex.lock();
    }

    ~InterprocessMutexLockGuard()
    {
        mutex.unlock();
    }

private:
    InterprocessMutex& mutex;
};

#endif
#ifndef CPPUTILS_INTERPROCESS_MUTEX_LOCK_GUARD_H
#define CPPUTILS_INTERPROCESS_MUTEX_LOCK_GUARD_H

#include "cpputils\InterprocessMutex.h"

namespace cpputils {

class InterprocessMutexLockGuard
{
public:
    InterprocessMutexLockGuard(InterprocessMutex& mutex) : mutex(mutex) { mutex.lock(); }

    ~InterprocessMutexLockGuard() { mutex.unlock(); }

private:
    InterprocessMutex& mutex;
};

} // namespace cpputils

#endif
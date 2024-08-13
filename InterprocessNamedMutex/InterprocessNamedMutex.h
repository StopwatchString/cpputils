#ifndef INTERPROCESS_NAMED_MUTEX_H
#define INTERPROCESS_NAMED_MUTEX_H

#include <string>
#include <iostream>

// Imports and SharedMemoryHandle struct are defined per-platform
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    struct SharedMutexHandle
    {
        HANDLE hMutex{ 0 };
    };
#elif defined(__linux__)

    struct SharedMutexHandle
    {

    };
#else
#error Unsupported Platform
#endif

// Forward declare functions which will be implemented per-platform
struct SharedMutexHandle;
bool platformAcquireMutexHandle(SharedMutexHandle& handle, const std::string& key);
bool platformReleaseMutexHandle(SharedMutexHandle& handle);
void platformLockMutex(SharedMutexHandle& handle);
void platformUnlockMutex(SharedMutexHandle& handle);
bool platformTryLockMutex(SharedMutexHandle& handle);

class InterprocessSharedMutex
{
public:
    InterprocessSharedMutex(const std::string& key)
    {
        // Initialize Members
        m_MutexKey = key;

        // Initialize Based on Members
        platformAcquireMutexHandle(m_SharedMutexHandle, m_MutexKey);
    }

    ~InterprocessSharedMutex()
    {
        if (m_Locked) {
            platformUnlockMutex(m_SharedMutexHandle);
        }
        platformReleaseMutexHandle(m_SharedMutexHandle);
    }

    void lock()
    {
        platformLockMutex(m_SharedMutexHandle);
        m_Locked = true;
    }

    void unlock()
    {
        platformUnlockMutex(m_SharedMutexHandle);
        m_Locked = false;
    }

    bool tryLock()
    {
        if (platformTryLockMutex(m_SharedMutexHandle)) {
            m_Locked = true;
        }
        return m_Locked;
    }

    bool locked()
    {
        return m_Locked;
    }

private:
    std::string m_MutexKey;
    bool m_Locked                           { false };
    SharedMutexHandle m_SharedMutexHandle   {};
};

#ifdef _WIN32
//#########################################################
// Windows Platform Implementation
//#########################################################

bool platformAcquireMutexHandle(SharedMutexHandle& handle, const std::string& key)
{

    if (key.length() > MAX_PATH) {
        std::cerr << "InterprocessNamedMutex Windows::platformAcquireMutexHandle() Key length is longer than MAX_PATH, the limit for a Windows platform mutex name." << '\n';
        return false;
    }
    handle.hMutex = CreateMutexA(NULL, FALSE, key.c_str());

    return handle.hMutex != NULL;
}

bool platformReleaseMutexHandle(SharedMutexHandle& handle)
{
    bool retVal = true;
    if (handle.hMutex != NULL) {
        retVal = CloseHandle(handle.hMutex);
        handle.hMutex = NULL;
    }
    return retVal;
}

void platformLockMutex(SharedMutexHandle& handle)
{
    WaitForSingleObject(handle.hMutex, INFINITE);
}

void platformUnlockMutex(SharedMutexHandle& handle)
{
    ReleaseMutex(handle.hMutex);
}

bool platformTryLockMutex(SharedMutexHandle& handle)
{
    return WaitForSingleObject(handle.hMutex, 0) == WAIT_OBJECT_0;
}

#elif defined(__linux__)
//#########################################################
// Linux Platform Implementation
//#########################################################

void platformAcquireMutexHandle(const std::string& key, SharedMutexHandle& handle)
{
    return false;
}

void platformReleaseMutexHandle(SharedMutexHandle& handle)
{
    return false;
}

void platformLockMutex(SharedMutexHandle& handle)
{
    return false;
}

void platformUnlockMutex(SharedMutexHandle& handle)
{
    return false;
}

#endif

#endif
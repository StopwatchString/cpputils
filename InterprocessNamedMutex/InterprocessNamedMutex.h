#ifndef INTERPROCESS_NAMED_MUTEX_H
#define INTERPROCESS_NAMED_MUTEX_H

#include <string>
#include <iostream>

// Imports and SharedMemoryHandle struct are defined per-platform
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    struct NamedMutexHandle
    {
        HANDLE hMutex{ 0 };
    };
#elif defined(__linux__)

    struct NamedMutexHandle
    {

    };
#else
#error Unsupported Platform
#endif

// Forward declare functions which will be implemented per-platform
bool platformAcquireMutexHandle(NamedMutexHandle& handle, const std::string& key);
bool platformReleaseMutexHandle(NamedMutexHandle& handle);
void platformLockMutex(NamedMutexHandle& handle);
void platformUnlockMutex(NamedMutexHandle& handle);
bool platformTryLockMutex(NamedMutexHandle& handle);

class InterprocessNamedMutex
{
public:

    //---------------------------------------------------------
    // Constructor
    //---------------------------------------------------------
    InterprocessNamedMutex(const std::string& key)
    {
        // Initialize Members
        m_MutexKey = key;

        // Initialize Based on Members
        platformAcquireMutexHandle(m_NamedMutexHandle, m_MutexKey);
    }

    //---------------------------------------------------------
    // Destructor
    //---------------------------------------------------------
    ~InterprocessNamedMutex()
    {
        if (m_Locked) {
            platformUnlockMutex(m_NamedMutexHandle);
        }
        platformReleaseMutexHandle(m_NamedMutexHandle);
    }

    //---------------------------------------------------------
    // Copy Constructor
    //---------------------------------------------------------
    InterprocessNamedMutex(const InterprocessNamedMutex& other)
    {
        // Initialize Members
        m_MutexKey = other.m_MutexKey;

        // Initialize Based on Members
        platformAcquireMutexHandle(m_NamedMutexHandle, m_MutexKey);
    }

    //---------------------------------------------------------
    // Move Constructor
    //---------------------------------------------------------
    InterprocessNamedMutex(InterprocessNamedMutex&& other) noexcept
    {
        // Initialize Members
        m_MutexKey = other.m_MutexKey;
        m_Locked = other.m_Locked;
        m_NamedMutexHandle = other.m_NamedMutexHandle;


        // Clean up other class
        other.m_MutexKey = "";
        other.m_Locked = false;
        other.m_NamedMutexHandle = NamedMutexHandle();
    }

    //---------------------------------------------------------
    // Copy Assignment
    //---------------------------------------------------------
    InterprocessNamedMutex& operator=(const InterprocessNamedMutex& other)
    {
        if (this != &other) {
            // Initialize Members
            m_MutexKey = other.m_MutexKey;

            // Initialize Based on Members
            platformAcquireMutexHandle(m_NamedMutexHandle, m_MutexKey);
        }
        return *this;
    }

    //---------------------------------------------------------
    // Move Assignment
    //---------------------------------------------------------
    InterprocessNamedMutex& operator=(InterprocessNamedMutex&& other) noexcept
    {
        if (this != &other) {
            // Initialize Members
            m_MutexKey = other.m_MutexKey;
            m_Locked = other.m_Locked;
            m_NamedMutexHandle = other.m_NamedMutexHandle;

            // Clean up other class
            other.m_MutexKey = "";
            other.m_Locked = false;
            other.m_NamedMutexHandle = NamedMutexHandle();
        }
        return *this;
    }

    //---------------------------------------------------------
    // lock()
    //---------------------------------------------------------
    void lock()
    {
        platformLockMutex(m_NamedMutexHandle);
        m_Locked = true;
    }

    //---------------------------------------------------------
    // unlock()
    //---------------------------------------------------------
    void unlock()
    {
        platformUnlockMutex(m_NamedMutexHandle);
        m_Locked = false;
    }

    //---------------------------------------------------------
    // tryLock()
    //---------------------------------------------------------
    bool tryLock()
    {
        m_Locked = platformTryLockMutex(m_NamedMutexHandle);
        return m_Locked;
    }

    //---------------------------------------------------------
    // locked()
    //---------------------------------------------------------
    bool locked()
    {
        return m_Locked;
    }

private:
    std::string m_MutexKey;
    bool m_Locked                           { false };
    NamedMutexHandle m_NamedMutexHandle   {};
};

#ifdef _WIN32
//#########################################################
// Windows Platform Implementation
//#########################################################

bool platformAcquireMutexHandle(NamedMutexHandle& handle, const std::string& key)
{

    if (key.length() > MAX_PATH) {
        std::cerr << "InterprocessNamedMutex Windows::platformAcquireMutexHandle() Key length is longer than MAX_PATH, the limit for a Windows platform mutex name." << '\n';
        return false;
    }
    handle.hMutex = CreateMutexA(NULL, FALSE, key.c_str());

    return handle.hMutex != NULL;
}

bool platformReleaseMutexHandle(NamedMutexHandle& handle)
{
    bool retVal = true;
    if (handle.hMutex != NULL) {
        retVal = CloseHandle(handle.hMutex);
        handle.hMutex = NULL;
    }
    return retVal;
}

void platformLockMutex(NamedMutexHandle& handle)
{
    WaitForSingleObject(handle.hMutex, INFINITE);
}

void platformUnlockMutex(NamedMutexHandle& handle)
{
    ReleaseMutex(handle.hMutex);
}

bool platformTryLockMutex(NamedMutexHandle& handle)
{
    return WaitForSingleObject(handle.hMutex, 0) == WAIT_OBJECT_0;
}

#elif defined(__linux__)
//#########################################################
// Linux Platform Implementation
//#########################################################

void platformAcquireMutexHandle(const std::string& key, NamedMutexHandle& handle)
{
    return false;
}

void platformReleaseMutexHandle(NamedMutexHandle& handle)
{
    return false;
}

void platformLockMutex(NamedMutexHandle& handle)
{
    return false;
}

void platformUnlockMutex(NamedMutexHandle& handle)
{
    return false;
}

#endif

#endif
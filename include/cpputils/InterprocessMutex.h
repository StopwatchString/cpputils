#ifndef CPPUTILS_INTERPROCESS_MUTEX_H
#define CPPUTILS_INTERPROCESS_MUTEX_H

#include <iostream>
#include <string>
#include <cstdint>

#if defined(_WIN32)
    #include <windows.h>
#elif defined (__linux__)
    // TODO:: Linux Implementation
#endif

namespace cpputils {

//------------------------------------------------------------
// class InterprocessMutex
//------------------------------------------------------------
class InterprocessMutex
{
public:
    InterprocessMutex(const std::string& key)
        : key(key),
          locked(false)
    {
        createInterprocessMutex();
    }

    ~InterprocessMutex()
    {
        closeInterprocessMutex();
    }

    void lock()
    {
#if defined(_WIN32)
        DWORD result = WaitForSingleObject(hMutex, INFINITE);
#elif defined (__linux__)
    // TODO:: Linux Implementation
#endif
        locked = true;
    }

    void unlock()
    {
#if defined(_WIN32)
        ReleaseMutex(hMutex);
#elif defined (__linux__)
    // TODO:: Linux Implementation
#endif
        locked = false;
    }

    bool tryLock()
    {
#if defined(_WIN32)
        locked = WaitForSingleObject(hMutex, 0) == WAIT_OBJECT_0;
#elif defined (__linux__)
    // TODO:: Linux Implementation
#endif
        return locked;
    }

private:
    std::string key;
    bool locked;

    // Platform-specific members
#if defined(_WIN32)
    HANDLE hMutex{ NULL };
#elif defined (__linux__)
    // TODO:: Linux Implementation
#endif

    void createInterprocessMutex()
    {
#if defined(_WIN32)
        hMutex = CreateMutexA(
            NULL,        // Security params, NULL = default security
            FALSE,       // Specifies if we immediately lock the mutex upon creation
            key.c_str()  // Key to create mutex as
        );

        if (hMutex == NULL) {
            std::cerr << "ERROR InterprocessMutex::createInterprocessMutex() (Windows) CreateMutexA returned NULL, could not create mutex!" << std::endl;
        }
#elif defined (__linux__)
    // TODO:: Linux Implementation
#endif
    }

    void closeInterprocessMutex()
    {
#if defined(_WIN32)
        if (hMutex != NULL) {
            CloseHandle(hMutex);
            hMutex = NULL;
        }
#elif defined (__linux__)
    // TODO:: Linux Implementation
#endif
    }

};

} // End cpputils namespace

#endif
#ifndef CPPUTILS_SHARED_LIBRARY_LOADER_H
#define CPPUTILS_SHARED_LIBRARY_LOADER_H

#if defined(_WIN32)
    #include <windows.h>
#elif defined(__linux__)
// TODO:: Linux Implementation
#endif

#include <string>
#include <iostream>

namespace cpputils {

//-----------------------------------------------------
// class SharedLibraryLoader
//
// Platform independent RAII shared library loader.
// Opens SharedLibrary from input name. Handles cleanup
// for library handle on destruction.
//-----------------------------------------------------
class SharedLibraryLoader
{
public:
    //-----------------------------------------------------
    // Constructor
    //-----------------------------------------------------
    SharedLibraryLoader(const std::string& libraryName) : libraryName(libraryName)
    {
#if defined(_WIN32)
        hLibModule = LoadLibraryA(libraryName.c_str());
        if (hLibModule == NULL) {
            std::cerr << "ERROR cpputils SharedLibraryLoader() Failed to load " << libraryName
                      << " GetLastError():" << GetLastError() << std::endl;
        }
#elif defined(__linux__)
            // TODO:: Linux Implementation
#endif
    }

    //-----------------------------------------------------
    // Destructor
    //-----------------------------------------------------
    ~SharedLibraryLoader()
    {
#if defined(_WIN32)
        if (hLibModule != NULL) {
            FreeLibrary(hLibModule);
            hLibModule = NULL;
        }
#elif defined(__linux__)
            // TODO:: Linux Implementation
#endif
    }

    //-----------------------------------------------------
    // loadFunctionPointer()
    //-----------------------------------------------------
    void* loadFunctionPointer(const char* functionName) const
    {
        void* funcPointer = nullptr;
#if defined(_WIN32)
        if (hLibModule != NULL) {
            funcPointer = GetProcAddress(hLibModule, functionName);
        }
#elif defined(__linux__)
            // TODO:: Linux Implementation
#endif
        return funcPointer;
    }

    //-----------------------------------------------------
    // valid()
    //-----------------------------------------------------
    bool valid() const
    {
#if defined(_WIN32)
        return hLibModule != NULL;
#elif defined(__linux__)
            // TODO:: Linux Implementation
#endif
    }

    //-----------------------------------------------------
    // name() - Returns libraryName passed to constructor
    //-----------------------------------------------------
    std::string name() const { return libraryName; }

private:
    const std::string libraryName;

    // Platform-specific member variables
#if defined(_WIN32)
    HMODULE hLibModule{NULL};
#elif defined(__linux__)
        // TODO:: Linux Implementation
#endif
};

} // namespace cpputils

#endif
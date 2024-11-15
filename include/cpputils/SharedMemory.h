#ifndef CPPUTILS_SHARED_MEMORY_H
#define CPPUTILS_SHARED_MEMORY_H

#include <iostream>
#include <string>
#include <cstdint>

#if defined(_WIN32)
    #include <windows.h>
#elif defined (__linux__)
    // TODO:: Linux Implementation
#else
    #error SharedMemory.h: Unsupported Platform
#endif
//------------------------------------------------------------
// class SharedMemory
//------------------------------------------------------------
class SharedMemory
{
public:
    SharedMemory(const std::string& key, size_t sizeBytes)
        : key(key),
          sizeBytes(sizeBytes)
    {
        openSharedMemory();
    }

    ~SharedMemory()
    {
        closeSharedMemory();
    }

    // No copy operations for now
    SharedMemory(const SharedMemory& other) = delete;
    SharedMemory& operator=(const SharedMemory& other) = delete;

    // No move operations for now
    SharedMemory(SharedMemory&& other) noexcept = delete;
    SharedMemory& operator=(SharedMemory&& other) noexcept = delete;

    void* data() { return pData; }
    size_t size() const { return sizeBytes; }

private:
    void* pData{ nullptr };
    std::string key;
    size_t sizeBytes{ 0 };

    // Platform-specific members
#if defined(_WIN32)
    HANDLE hFileMapping{ NULL };
#elif defined(__linux__)
    // TODO:: Linux Implementation
#else
    #error SharedMemory.h: Unsupported Platform
#endif

    //------------------------------------------------------------
    // openSharedMemory()
    //------------------------------------------------------------
    void openSharedMemory()
    {
#if defined(_WIN32)
        uint32_t lower = static_cast<uint32_t>(sizeBytes & 0xFFFFFFFF);
        uint32_t upper = static_cast<uint32_t>((sizeBytes >> 32) & 0xFFFFFFFF);

        hFileMapping = CreateFileMappingA(
            INVALID_HANDLE_VALUE,  // File backing the mapping. When NULL, system paging file is used (shared memory)
            NULL,                  // Security params, NULL == default security
            PAGE_READWRITE,        // Access params for file mappings
            upper,                 // High DWORD indicating size
            lower,                 // Low DWORD indicating size
            key.c_str()            // Key used to identify this mapping
        );

        if (hFileMapping == NULL) {
            std::cerr << "ERROR SharedMemory::openSharedMemory() (Windows) CreateFileMappingA returned NULL, could not open file!" << std::endl;
            return;
        }

        pData = MapViewOfFile(
            hFileMapping,         // File mapping to map
            FILE_MAP_ALL_ACCESS,  // Access params for mapped view
            0,                    // High DWORD offset into file
            0,                    // Low DWORD offset into file
            sizeBytes             // Size in bytes to map
        );

        if (pData == nullptr) {
            std::cerr << "ERROR SharedMemory::openSharedMemory() (Windows) MapViewOfFile returned nullptr. Could not get pointer to shared memory!" << std::endl;
            CloseHandle(hFileMapping);
            hFileMapping = NULL;
            return;
        }

#elif defined (__linux__)
    // TODO:: Linux Implementation
#else
    #error SharedMemory: Unsupported Platform
#endif
    }

    //------------------------------------------------------------
    // closeSharedMemory()
    //------------------------------------------------------------
    void closeSharedMemory()
    {
#if defined(_WIN32)
        if (pData != nullptr) {
            UnmapViewOfFile(pData);
            pData = nullptr;
        }

        if (hFileMapping != NULL) {
            CloseHandle(hFileMapping);
            hFileMapping = NULL;
        }

#elif defined (__linux__)
    // TODO:: Linux Implementation
#else
    #error SharedMemory.h: Unsupported Platform
#endif
    }
};

#endif
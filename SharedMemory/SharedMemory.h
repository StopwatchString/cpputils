#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <string>
#include <iostream>

#ifdef _WIN32
//#########################################################
// Windows Shared Memory Interface
//#########################################################

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct SharedMemoryHandle
{
    HANDLE hMapFile = NULL;
    void* pData = nullptr;
};

//---------------------------------------------------------
// openSharedMemory
//---------------------------------------------------------
void openSharedMemory(const std::string& key, const uint32_t sizeBytes, SharedMemoryHandle& sharedMemoryHandle)
{
    sharedMemoryHandle.hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,         // Use paging file
        NULL,                         // Default security
        PAGE_READWRITE,               // Read/write access
        0,                            // Maximum object size (high-order DWORD)
        sizeBytes,                    // Maximum object size (low-order DWORD)
        key.c_str());                 // Name of mapping object

    if (sharedMemoryHandle.hMapFile == NULL) {
        std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
        return;
    }

    sharedMemoryHandle.pData = MapViewOfFile(
        sharedMemoryHandle.hMapFile,  // Handle to map object
        FILE_MAP_ALL_ACCESS,          // Read/write permission
        0,                            // Offset high
        0,                            // Offset low
        sizeBytes);                   // Number of bytes to map

    if (sharedMemoryHandle.pData == nullptr) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        CloseHandle(sharedMemoryHandle.hMapFile);
        sharedMemoryHandle.hMapFile = NULL;
    }
}

//---------------------------------------------------------
// closeSharedMemory
//---------------------------------------------------------
void closeSharedMemory(SharedMemoryHandle& smh)
{
    if (smh.pData != nullptr) {
        UnmapViewOfFile(smh.pData);
        smh.pData = nullptr;
    }

    if (smh.hMapFile != NULL) {
        CloseHandle(smh.hMapFile);
        smh.hMapFile = NULL;
    }
}

#else
//#########################################################
// Linux Shared Memory Interface
//#########################################################

struct SharedMemoryHandle
{
    void* pData = nullptr;
};

//---------------------------------------------------------
// openSharedMemory
//---------------------------------------------------------
void openSharedMemory(const std::string& key, const uint32_t sizeBytes, SharedMemoryHandle& sharedMemoryHandle)
{
}

//---------------------------------------------------------
// closeSharedMemory
//---------------------------------------------------------
void closeSharedMemory(SharedMemoryHandle& smh)
{
}

#endif

//#########################################################
// SharedMemory
//#########################################################
template <typename _DataType>
class SharedMemory
{
public:
    //---------------------------------------------------------
    // Constructor
    //---------------------------------------------------------
    SharedMemory(const std::string& key)
    {
        // Initialize Members
        m_DataKey = key;
        m_DataSize = sizeof(_DataType);
        m_SharedMemoryHandle = SharedMemoryHandle();

        // Initialize Based on Members
        openSharedMemory(m_DataKey, m_DataSize, m_SharedMemoryHandle);
    }

    //---------------------------------------------------------
    // Destructor
    //---------------------------------------------------------
    ~SharedMemory()
    {
        closeSharedMemory(m_SharedMemoryHandle);
    }

    //---------------------------------------------------------
    // Copy Constructor
    //---------------------------------------------------------
    SharedMemory(const SharedMemory<_DataType>& other)
    {
        // Initialize Members
        m_DataKey = other.m_DataKey;
        m_DataSize = sizeof(_DataType);
        m_SharedMemoryHandle = SharedMemoryHandle();

        // Initialize Based on Members
        openSharedMemory(m_DataKey, m_DataSize, m_SharedMemoryHandle);

        // No need to copy underlying data since we're opening the same shared memory address.
    }

    //---------------------------------------------------------
    // Move Constructor
    //---------------------------------------------------------
    SharedMemory(SharedMemory<_DataType>&& other)
    {
        // Initialize Members
        m_DataKey = other.m_DataKey;
        m_DataSize = sizeof(_DataType);
        m_SharedMemoryHandle = other.m_SharedMemoryHandle;

        // Clean up other class
        other.m_DataKey = "";
        other.m_DataSize = 0;
        other.m_SharedMemoryHandle = SharedMemoryHandle();

        // No need to initialize shared memory since we take control of other's handles.
    }

    //---------------------------------------------------------
    // operator= Copy
    //---------------------------------------------------------
    SharedMemory<_DataType>& operator=(const SharedMemory<_DataType>& other)
    {
        // Clean up our resources
        closeSharedMemory(m_SharedMemoryHandle);

        // Initialize Members
        m_DataKey = other.m_DataKey;

        // Initialize Based on Members
        openSharedMemory(m_DataKey, m_DataSize, m_SharedMemoryHandle);

        // No need to copy underlying data since we're opening the same shared memory address.

        return *this;
    }

    //---------------------------------------------------------
    // operator= Move
    //---------------------------------------------------------
    SharedMemory<_DataType>& operator=(SharedMemory<_DataType>&& other) noexcept
    {
        if (this != &other) {
            // Clean up our resources
            closeSharedMemory(m_SharedMemoryHandle);

            // Initialize Members
            m_DataKey = other.m_DataKey;
            m_SharedMemoryHandle = other.m_SharedMemoryHandle;

            // Clean up other class
            other.m_DataKey = "";
            other.m_DataSize = 0;
            other.m_SharedMemoryHandle = SharedMemoryHandle();
        }

        return *this;
    }

    //---------------------------------------------------------
    // operator= Special case
    //---------------------------------------------------------
    SharedMemory<_DataType>& operator=(const _DataType& other)
    {
        *static_cast<_DataType*>(m_SharedMemoryHandle.pData) = other;
        return *this;
    }

    //---------------------------------------------------------
    // operator->
    //---------------------------------------------------------
    _DataType* operator->() const
    {
        return static_cast<_DataType*>(m_SharedMemoryHandle.pData);
    }

    //---------------------------------------------------------
    // operator cast to _DataType
    //---------------------------------------------------------
    operator _DataType() const
    {
        return *static_cast<_DataType*>(m_SharedMemoryHandle.pData);
    }

    // Returns pointer to raw data, pretyped to template type
    _DataType* data()     { return static_cast<_DataType*>(m_SharedMemoryHandle.pData); }
    // Returns the key used to open this instance of shared memory
    std::string key()     { return m_DataKey; }
    // Returns size of SharedMemory in bytes
    uint32_t size()       { return m_DataSize; }

private:
    std::string               m_DataKey            {};
    uint32_t                  m_DataSize           { 0 };
    SharedMemoryHandle        m_SharedMemoryHandle {};
};
#endif
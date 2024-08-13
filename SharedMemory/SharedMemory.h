#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <string>
#include <iostream>

// Windows vs Linux Include
#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    //LINUX
#endif

// Windows vs Linux Implementation
#ifdef WIN32
struct SharedMemoryHandle
{
    HANDLE hMapFile = 0;
    void* pData = nullptr;
};

//---------------------------------------------------------
// openSharedMemory
//---------------------------------------------------------
void openSharedMemory(const std::string& key, const uint32_t sizeBytes, SharedMemoryHandle& sharedMemoryHandle)
{
    sharedMemoryHandle.hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,       // Use paging file
        NULL,                       // Default security
        PAGE_READWRITE,             // Read/write access
        0,                          // Maximum object size (high-order DWORD)
        sizeBytes,                  // Maximum object size (low-order DWORD)
        key.c_str());               // Name of mapping object

    if (sharedMemoryHandle.hMapFile == NULL) {
        std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
    }

    sharedMemoryHandle.pData = MapViewOfFile(
        sharedMemoryHandle.hMapFile,               // Handle to map object
        FILE_MAP_ALL_ACCESS,        // Read/write permission
        0,                          // Offset high
        0,                          // Offset low
        sizeBytes);                 // Number of bytes to map

    if (sharedMemoryHandle.pData == nullptr) {
        std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
        CloseHandle(sharedMemoryHandle.hMapFile);
        sharedMemoryHandle.hMapFile = 0;
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

    if (smh.hMapFile != 0) {
        CloseHandle(smh.hMapFile);
        smh.hMapFile = 0;
    }
}
#else
    //LINUX
#endif

//---------------------------------------------------------
// Generic Template
//---------------------------------------------------------
template <typename _DataType = void>
class SharedMemory
{
public:
    //---------------------------------------------------------
    // Constructor
    //---------------------------------------------------------
    SharedMemory(const std::string& key) : m_DataKey(key)
    {
        openSharedMemory(m_DataKey, m_DataSize, m_SharedMemoryHandle);
        m_DataPointer = static_cast<_DataType*>(m_SharedMemoryHandle.pData);
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
    SharedMemory(const SharedMemory<_DataType>& other) : m_DataKey(other.m_DataKey)
    {
        openSharedMemory(m_DataKey, m_DataSize, m_SharedMemoryHandle);
        m_DataPointer = static_cast<_DataType*>(m_SharedMemoryHandle.pData);
        // No need to copy underlying data since we're opening the same shared memory address.
    }

    //---------------------------------------------------------
    // Move Constructor
    //---------------------------------------------------------
    SharedMemory(SharedMemory<_DataType>&& other) : m_DataKey(other.m_DataKey)
    {
        // Manage ourselves first to guarantee SharedMemory lives
        closeSharedMemory(m_SharedMemoryHandle);
        openSharedMemory(m_DataKey, m_DataSize, m_SharedMemoryHandle);
        m_DataPointer = static_cast<_DataType*>(m_SharedMemoryHandle.pData);

        // Clean up other class
        closeSharedMemory(other.m_SharedMemoryHandle);
        other.m_DataKey = "";
        other.m_DataPointer = nullptr;
        other.m_DataSize = 0;
    }

    //---------------------------------------------------------
    // operator= Copy
    //---------------------------------------------------------
    SharedMemory<_DataType>& operator=(const SharedMemory<_DataType>& other)
    {
        *m_DataPointer = *other.m_DataPointer;
        return *this;
    }

    //---------------------------------------------------------
    // operator= Move
    //---------------------------------------------------------
    //SharedMemory<_DataType>& operator=(const SharedMemory<_DataType>&& other)
    //{
    //    *m_DataPointer = other;
    //    return *this;
    //}

    //---------------------------------------------------------
    // operator= Special case
    //---------------------------------------------------------
    SharedMemory<_DataType>& operator=(const _DataType& other)
    {
        *m_DataPointer = other;
        return *this;
    }

    //---------------------------------------------------------
    // operator->
    //---------------------------------------------------------
    _DataType* operator->()
    {
        return m_DataPointer;
    }

    //---------------------------------------------------------
    // operator cast to _DataType
    //---------------------------------------------------------
    operator _DataType() const
    {
        return *m_DataPointer;
    }

    _DataType* data()     { return m_DataPointer; }
    uint32_t size()       { return m_DataSize; }
    std::string key()     { return m_DataKey; }
    bool isOpen()         { return m_DataPointer != nullptr; }

private:

    SharedMemoryHandle        m_SharedMemoryHandle {};
    std::string               m_DataKey            {};
    _DataType*                m_DataPointer        { nullptr };
    uint32_t                  m_DataSize           { sizeof(_DataType) };
};


//---------------------------------------------------------
// Void Template
//---------------------------------------------------------
template<>
class SharedMemory<>
{
public:
    //---------------------------------------------------------
    // Constructor
    //---------------------------------------------------------
    SharedMemory(const std::string& key, const uint32_t sizeBytes) : m_DataKey(key), m_DataSize(sizeBytes)
    {
        openSharedMemory(key, sizeBytes, m_SharedMemoryHandle);
    }

    //---------------------------------------------------------
    // Destructor
    //---------------------------------------------------------
    ~SharedMemory()
    {
        closeSharedMemory(m_SharedMemoryHandle);
    }

    void* data()      { return m_SharedMemoryHandle.pData; }
    uint32_t size()   { return m_DataSize; }
    std::string key() { return m_DataKey; }
    bool isOpen()     { return m_SharedMemoryHandle.pData != nullptr; }

private:

    SharedMemoryHandle m_SharedMemoryHandle{};
    std::string        m_DataKey{};
    uint32_t           m_DataSize{ 0 };
};

#endif
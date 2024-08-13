#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H

#include <string>
#include <iostream>

#ifdef WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
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
        openSharedMemory();
    }

    //---------------------------------------------------------
    // Destructor
    //---------------------------------------------------------
    ~SharedMemory()
    {
        closeSharedMemory();
    }

    //---------------------------------------------------------
    // Copy Constructor
    //---------------------------------------------------------
    SharedMemory(const SharedMemory<_DataType>& other) : m_DataKey(other.m_DataKey)
    {
        openSharedMemory();
        // No need to copy underlying data since we're opening the same shared memory address.
    }

    //---------------------------------------------------------
    // Move Constructor
    //---------------------------------------------------------
    SharedMemory(SharedMemory<_DataType>&& other)
    {
        closeSharedMemory();
        m_DataKey = other.m_DataKey;
        openSharedMemory();

        // Close the other's handles for it
#ifdef WIN32
        if (other.m_DataPointer != nullptr) {
            UnmapViewOfFile(other.m_DataPointer);
            other.m_DataPointer = nullptr;
        }

        if (other.m_hMapFile != 0) {
            CloseHandle(other.m_hMapFile);
            other.m_hMapFile = 0;
        }
#else
        //LINUX
#endif
        other.m_DataKey = "";
        other.m_DataSize = 0;
        std::cout << "Moved!" << std::endl;
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
    //---------------------------------------------------------
    // openSharedMemory()
    //---------------------------------------------------------
    void openSharedMemory()
    {
#ifdef WIN32
        m_hMapFile = CreateFileMappingA(
            INVALID_HANDLE_VALUE,       // Use paging file
            NULL,                       // Default security
            PAGE_READWRITE,             // Read/write access
            0,                          // Maximum object size (high-order DWORD)
            m_DataSize,                 // Maximum object size (low-order DWORD)
            m_DataKey.c_str());         // Name of mapping object

        if (m_hMapFile == NULL) {
            std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
            return;
        }

        void* hMapView = MapViewOfFile(
            m_hMapFile,                 // Handle to map object
            FILE_MAP_ALL_ACCESS,        // Read/write permission
            0,                          // Offset high
            0,                          // Offset low
            m_DataSize);                // Number of bytes to map
        m_DataPointer = reinterpret_cast<_DataType*>(hMapView);

        if (m_DataPointer == nullptr) {
            std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
            CloseHandle(m_hMapFile);
            return;
        }
#else
        //LINUX
#endif
    }

    //---------------------------------------------------------
    // closeSharedMemory()
    //---------------------------------------------------------
    void closeSharedMemory()
    {
#ifdef WIN32
        if (m_DataPointer != nullptr) {
            UnmapViewOfFile(m_DataPointer);
            m_DataPointer = nullptr;
        }

        if (m_hMapFile != 0) {
            CloseHandle(m_hMapFile);
            m_hMapFile = 0;
        }
#else
        //LINUX
#endif
    }

#ifdef WIN32
    HANDLE      m_hMapFile    { 0 };
#else
    //LINUX
#endif

    std::string               m_DataKey     {};
    _DataType*                m_DataPointer { nullptr };
    uint32_t                  m_DataSize    { sizeof(_DataType) };
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
        openSharedMemory();
    }

    //---------------------------------------------------------
    // Constructor
    //---------------------------------------------------------
    ~SharedMemory()
    {
        closeSharedMemory();
    }

    void* data()      { return m_DataPointer; }
    uint32_t size()   { return m_DataSize; }
    std::string key() { return m_DataKey; }
    bool isOpen()     { return m_DataPointer != nullptr; }

private:

    //---------------------------------------------------------
    // openSharedMemory()
    //---------------------------------------------------------
    void openSharedMemory()
    {
#ifdef WIN32
        m_hMapFile = CreateFileMappingA(
            INVALID_HANDLE_VALUE,       // Use paging file
            NULL,                       // Default security
            PAGE_READWRITE,             // Read/write access
            0,                          // Maximum object size (high-order DWORD)
            m_DataSize,                 // Maximum object size (low-order DWORD)
            m_DataKey.c_str());         // Name of mapping object

        if (m_hMapFile == NULL) {
            std::cerr << "Could not create file mapping object: " << GetLastError() << std::endl;
            return;
        }

        m_DataPointer = MapViewOfFile(
            m_hMapFile,                 // Handle to map object
            FILE_MAP_ALL_ACCESS,        // Read/write permission
            0,                          // Offset high
            0,                          // Offset low
            m_DataSize);                // Number of bytes to map

        if (m_DataPointer == nullptr) {
            std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
            CloseHandle(m_hMapFile);
            return;
        }
#else
        //LINUX
#endif
    }

    //---------------------------------------------------------
    // closeSharedMemory()
    //---------------------------------------------------------
    void closeSharedMemory()
    {
#ifdef WIN32
        if (m_DataPointer != nullptr) {
            UnmapViewOfFile(m_DataPointer);
            m_DataPointer = nullptr;
        }

        if (m_hMapFile != 0) {
            CloseHandle(m_hMapFile);
            m_hMapFile = 0;
        }
#else
        //LINUX
#endif
    }

#ifdef WIN32
    HANDLE      m_hMapFile{ 0 };
#else
    //LINUX
#endif

    std::string    m_DataKey{};
    void*          m_DataPointer{ nullptr };
    uint32_t       m_DataSize{ 0 };
};

#endif
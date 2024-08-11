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

template <typename _DataType = void>
class SharedMemory
{
public:
    SharedMemory(const std::string& key) : m_DataKey(key)
    {
#ifdef WIN32
        // Create or open the shared memory
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

        // Map the shared memory into the address space of our process
        m_DataPointer = MapViewOfFile(
            m_hMapFile,                   // Handle to map object
            FILE_MAP_ALL_ACCESS,        // Read/write permission
            0,                          // Offset high
            0,                          // Offset low
            m_DataSize);         // Number of bytes to map

        if (m_DataPointer == nullptr) {
            std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
            CloseHandle(m_hMapFile);
            return;
        }
#else
    //LINUX
#endif
    }

    ~SharedMemory()
    {
#ifdef WIN32
        if (m_DataPointer != nullptr) {
            UnmapViewOfFile(m_DataPointer);
        }

        if (m_hMapFile != 0) {
            CloseHandle(m_hMapFile);
        }
#else
    //LINUX
#endif
    }

    _DataType* data()     { return (_DataType*)m_DataPointer; }
    uint32_t size()       { return m_DataSize; }
    bool isOpen()         { return m_DataPointer != nullptr; }

private:
#ifdef WIN32
    HANDLE      m_hMapFile    { 0 };
#else
    //LINUX
#endif

    std::string               m_DataKey     {};
    void*                     m_DataPointer { nullptr };
    static constexpr uint32_t m_DataSize{ sizeof(_DataType) };
};

template<>
class SharedMemory<>
{
public:
    SharedMemory(const std::string& key, const uint32_t sizeBytes) : m_DataKey(key), m_DataSize(sizeBytes)
    {
#ifdef WIN32
        // Create or open the shared memory
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

        // Map the shared memory into the address space of our process
        m_DataPointer = MapViewOfFile(
            m_hMapFile,                   // Handle to map object
            FILE_MAP_ALL_ACCESS,        // Read/write permission
            0,                          // Offset high
            0,                          // Offset low
            m_DataSize);         // Number of bytes to map

        if (m_DataPointer == nullptr) {
            std::cerr << "Could not map view of file: " << GetLastError() << std::endl;
            CloseHandle(m_hMapFile);
            return;
        }
#else
    //LINUX
#endif
    }

    ~SharedMemory()
    {
#ifdef WIN32
        if (m_DataPointer != nullptr) {
            UnmapViewOfFile(m_DataPointer);
        }

        if (m_hMapFile != 0) {
            CloseHandle(m_hMapFile);
        }
#else
    //LINUX
#endif
    }

    void* data()    { return m_DataPointer; }
    uint32_t size() { return m_DataSize; }
    bool isOpen()   { return m_DataPointer != nullptr; }

private:
#ifdef WIN32
    HANDLE      m_hMapFile{ 0 };
#else
    //LINUX
#endif

    std::string    m_DataKey{};
    void*          m_DataPointer{ nullptr };
    const uint32_t m_DataSize{ 0 };
};

#endif
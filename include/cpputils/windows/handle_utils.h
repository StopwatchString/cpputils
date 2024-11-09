#ifndef CPPUTILS_WINDOWS_HANDLE_UTILS_H
#define CPPUTILS_WINDOWS_HANDLE_UTILS_H

#include "cpputils/SharedMemory.h"

#include <windows.h>

#include <string>
#include <iostream>

struct ShareHandleData
{
    HANDLE handle{ NULL };
    DWORD sourceProcessId{ 0 };
    bool ready{ false };
};

class EmittedHandle
{
public:
    EmittedHandle(const std::string& key, HANDLE handle)
        : sharedMemory(key, sizeof(ShareHandleData))
    {
        data.handle = handle;
        data.sourceProcessId = GetCurrentProcessId();
        data.ready = true;
        if (sharedMemory.data() != nullptr) {
            memcpy(sharedMemory.data(), &data, sizeof(ShareHandleData));
        }
    }

private:
    SharedMemory sharedMemory;
    ShareHandleData data;
};

HANDLE getHandleCopy(const std::string& key)
{
    // If OpenFileMapping is used here, then MapViewOfFile always fails below
    // We use a flag to indicate 'readiness' in the mapped memory instead (windows guarantees fresh shared memory is zeroed)
    //HANDLE hFileMapping = OpenFileMappingA(
    //    PAGE_READWRITE,
    //    FALSE,
    //    key.c_str()
    //);

    HANDLE hFileMapping = CreateFileMappingA(
        NULL,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(ShareHandleData),
        key.c_str()
    );

    if (hFileMapping == NULL) {
        std::cerr << "ERROR getHandleCopy() Could not open file mapping!" << std::endl;
        return NULL;
    }

    void* pData = MapViewOfFile(
        hFileMapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(ShareHandleData)
    );

    if (pData == nullptr) {
        CloseHandle(hFileMapping);
        std::cerr << "ERROR getHandleCopy() Could not map view of file!" << std::endl;
        return NULL;
    }

    ShareHandleData data;
    memcpy(&data, pData, sizeof(ShareHandleData));

    if (!data.ready) {
        std::cerr << "ERROR getHandleCopy() Mapped data but it's not ready!" << std::endl;
        UnmapViewOfFile(pData);
        CloseHandle(hFileMapping);
        return NULL;
    }

    HANDLE hSourceProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, data.sourceProcessId);
    if (hSourceProcess == NULL) {
        UnmapViewOfFile(pData);
        CloseHandle(hFileMapping);
        std::cout << "ERROR getHandleCopy() Could not get handle for source process!" << std::endl;
        return NULL;
    }

    HANDLE hDuplicate = NULL;
    DuplicateHandle(
        hSourceProcess,
        data.handle,
        GetCurrentProcess(),
        &hDuplicate,
        NULL,
        FALSE,
        DUPLICATE_SAME_ACCESS
    );

    if (hDuplicate == NULL) {
        std::cerr << "ERROR getHandleCopy() Could not create a duplicate of handle!" << std::endl;
    }

    CloseHandle(hSourceProcess);
    UnmapViewOfFile(pData);
    CloseHandle(hFileMapping);

    return hDuplicate;
}

#endif
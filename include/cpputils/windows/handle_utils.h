/**
* @file handle_utils.h
* @author Mason Speck
*/

#ifndef CPPUTILS_WINDOWS_HANDLE_UTILS_H
#define CPPUTILS_WINDOWS_HANDLE_UTILS_H

#include <windows.h>

#include <iostream>

namespace cpputils {
namespace windows {

/**
 * \brief RAII class to manage closing a Windows HANDLE automatically.
 * 
 * Cannot be copied, but supports move operations.
 */
class AutoHandle
{
public:
    AutoHandle(HANDLE handle) : m_Handle(handle) {}
    ~AutoHandle() { CloseHandle(m_Handle); }

    // No copy
    AutoHandle(const AutoHandle& other) = delete;
    AutoHandle& operator=(const AutoHandle& other) = delete;

    AutoHandle(AutoHandle&& other) noexcept : m_Handle(other.m_Handle) { other.m_Handle = nullptr; }
    AutoHandle& operator=(AutoHandle&& other) noexcept
    {
        if (this != &other) {
            m_Handle = other.m_Handle;
            other.m_Handle = nullptr;
        }
    }

private:
    HANDLE m_Handle{NULL};
};

//-----------------------------------------------
// duplicateHandle()
//-----------------------------------------------
static HANDLE duplicateHandle(DWORD sourceProcessId, HANDLE hSourceHandle)
{
    HANDLE hSourceProcessHandle = OpenProcess(
        PROCESS_DUP_HANDLE, // dwDesiredAccess
        FALSE,              // bInheritHandle
        sourceProcessId     // dwProcessId
    );
    if (hSourceProcessHandle == NULL) {
        std::cerr << "ERROR duplicateHandle() Could not get handle for source process!" << std::endl;
        return NULL;
    }

    HANDLE hTargetHandle = NULL;
    DuplicateHandle(
        hSourceProcessHandle, // hSourceProcessHandle
        hSourceHandle,        // hSourceHandle
        GetCurrentProcess(),  // hTargetProcessHandle
        &hTargetHandle,       // lpTargetHandle
        NULL,                 // dwDesiredAccess
        FALSE,                // bInheritHandle
        DUPLICATE_SAME_ACCESS // dwOptions
    );
    if (hTargetHandle == NULL) {
        std::cerr << "ERROR duplicateHandle() Could not create a duplicate of handle!" << std::endl;
    }

    CloseHandle(hSourceProcessHandle);

    return hTargetHandle;
}

//-----------------------------------------------
// getSourceProcessHandleFromHwnd()
//-----------------------------------------------
static HANDLE getSourceProcessHandleFromHwnd(HWND hWnd)
{
    LPDWORD lpdwProcessId = NULL;
    DWORD threadId = GetWindowThreadProcessId(hWnd, lpdwProcessId);

    HANDLE hProcess = NULL;
    if (lpdwProcessId != NULL) {
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, *lpdwProcessId);
    }

    return hProcess;
}

} // namespace windows
} // namespace cpputils

#endif
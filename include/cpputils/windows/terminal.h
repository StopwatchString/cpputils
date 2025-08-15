#ifndef CPPUTILS_WINDOWS_TERMINAL_H
#define CPPUTILS_WINDOWS_TERMINAL_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace cpputils {
namespace windows {

bool enableConsoleVirtualTerminalProcessing() {
    static bool windowsVirtualTerminalProcessingInit = false;
    if (!windowsVirtualTerminalProcessingInit) {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return false;

        DWORD dwMode = 0;
        if (!GetConsoleMode(hOut, &dwMode)) return false;

        // Enable the virtual terminal processing flag
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        windowsVirtualTerminalProcessingInit = SetConsoleMode(hOut, dwMode);
    }
    return windowsVirtualTerminalProcessingInit;
}

} // End windows
} // End cpputils

#endif

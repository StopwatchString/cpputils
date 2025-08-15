#ifndef CPPUTILS_WINDOWS_TERMINAL_H
#define CPPUTILS_WINDOWS_TERMINAL_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace cpputils {
namespace windows {

bool setConsoleVirtualTerminalProcessing(bool enabled)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return false;

    // Enable the virtual terminal processing flag
    if (enabled) {
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }
    else {
        dwMode &= ~ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }
    return  SetConsoleMode(hOut, dwMode);
}

bool setConsoleEOLWrapping(bool enabled)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return false;

    if (enabled) {
        dwMode |= ENABLE_WRAP_AT_EOL_OUTPUT;
    }
    else {
        dwMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
    }
    return SetConsoleMode(hOut, dwMode);
}

bool setConsoleDisableNewlineAutoReturn(bool enabled)
{
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return false;

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return false;

    if (enabled) {
        dwMode |= DISABLE_NEWLINE_AUTO_RETURN;
    }
    else {
        dwMode &= ~DISABLE_NEWLINE_AUTO_RETURN;
    }
    return SetConsoleMode(hOut, dwMode);
}

size_t getConsoleColumns()
{
    auto width_from = [](HANDLE h) -> int {
        CONSOLE_SCREEN_BUFFER_INFO csbi{};
        if (GetConsoleScreenBufferInfo(h, &csbi)) {
            return static_cast<int>(csbi.srWindow.Right - csbi.srWindow.Left + 1);
        }
        return 0;
        };

    // Try STDOUT first (may be a pipe)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        if (DWORD type = GetFileType(hOut); type == FILE_TYPE_CHAR) {
            if (int w = width_from(hOut)) return w;
        }
    }

    // Fallback to the active screen buffer even if stdout is redirected
    HANDLE hConOut = CreateFileW(L"CONOUT$",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        nullptr, OPEN_EXISTING, 0, nullptr);
    if (hConOut != INVALID_HANDLE_VALUE) {
        int w = width_from(hConOut);
        CloseHandle(hConOut);
        return w;
    }
    return 0; // no console
}

} // End windows
} // End cpputils

#endif

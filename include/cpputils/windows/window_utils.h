#ifndef CPPUTILS_WINDOW_UTILS_H
#define CPPUTILS_WINDOW_UTILS_H

#include <windows.h>
#include <dwmapi.h>

//-----------------------------------------------
// setWindowDarkMode()
//-----------------------------------------------
HRESULT setWindowDarkMode(HWND hWnd)
{
    BOOL value = TRUE;
    HRESULT hr = DwmSetWindowAttribute(
        hWnd,                           // hwnd
        DWMWA_USE_IMMERSIVE_DARK_MODE,  // dwAttribute
        &value,                         // pvAttribute
        sizeof(value)                   // cbAttribute
    );
    return hr;
}

#endif
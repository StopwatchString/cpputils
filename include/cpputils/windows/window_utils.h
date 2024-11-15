#ifndef CPPUTILS_WINDOWS_WINDOW_UTILS_H
#define CPPUTILS_WINDOWS_WINDOW_UTILS_H

#include <windows.h>
#include <dwmapi.h>

//-----------------------------------------------------
// window_utils.h
// 
// A set of utility functions for operating on native
// Windows window handles.
// 
// Must link against Dwmapi.lib to use these functions.
//-----------------------------------------------------

//-----------------------------------------------------
// setWindowDarkMode()
//-----------------------------------------------------
static HRESULT setWindowDarkMode(HWND hWnd, bool darkMode)
{
    BOOL value = darkMode ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttribute(
        hWnd,                           // hwnd
        DWMWA_USE_IMMERSIVE_DARK_MODE,  // dwAttribute
        &value,                         // pvAttribute
        sizeof(value)                   // cbAttribute
    );
    return hr;
}

//-----------------------------------------------------
// setWindowRoundedCorners()
//-----------------------------------------------------
static HRESULT setWindowRoundedCorners(HWND hWnd, bool rounded)
{
    DWM_WINDOW_CORNER_PREFERENCE pref = rounded ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    HRESULT hr = DwmSetWindowAttribute(
        hWnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &pref,
        sizeof(pref)
    );
    return hr;
}

//-----------------------------------------------------
// setWindowCloaked()
//-----------------------------------------------------
static HRESULT setWindowCloaked(HWND hWnd, bool cloaked)
{
    BOOL value = cloaked ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttribute(
        hWnd,
        DWMWA_CLOAK,
        &value,
        sizeof(value)
    );
    return hr;
}

//-----------------------------------------------------
// setWindowTitlebarTransparent()
//-----------------------------------------------------
static HRESULT setWindowTitlebarTransparent(HWND hWnd, bool transparent)
{
    DWM_SYSTEMBACKDROP_TYPE val = transparent ? DWMSBT_TRANSIENTWINDOW : DWMSBT_MAINWINDOW;
    HRESULT hr = DwmSetWindowAttribute(
        hWnd,
        DWMWA_SYSTEMBACKDROP_TYPE,
        &val,
        sizeof(val)
    );
    return hr;
}

#endif
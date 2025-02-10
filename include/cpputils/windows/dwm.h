#ifndef CPPUTILS_WINDOWS_DWM_H
#define CPPUTILS_WINDOWS_DWM_H

//-----------------------------------------------------
// dwm.h
//
// A set of functions to handle interaction with the
// Desktop Window Mangement API for you.
//-----------------------------------------------------

#include "cpputils/SharedLibraryLoader.h"

#include <windows.h>
#include <dwmapi.h>

namespace cpputils {
namespace windows {

const std::string DWM_API_DLL_NAME{"dwmapi.dll"};
static SharedLibraryLoader dwmApiLibrary(DWM_API_DLL_NAME);

//-----------------------------------------------------
// PFN_DWM_SET_WINDOW_ATTRIBUTE
//-----------------------------------------------------
typedef HRESULT(WINAPI* PFN_DWM_SET_WINDOW_ATTRIBUTE)(
    HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute);
static PFN_DWM_SET_WINDOW_ATTRIBUTE DwmSetWindowAttributePtr = nullptr;

//-----------------------------------------------------
// setWindowDarkMode()
//-----------------------------------------------------
static bool setWindowDarkMode(HWND hWnd, bool darkMode)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr
            = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiLibrary.loadFunctionPointer("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr)
        return false;

    BOOL value = darkMode ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,                          // hwnd
        DWMWA_USE_IMMERSIVE_DARK_MODE, // dwAttribute
        &value,                        // pvAttribute
        sizeof(value)                  // cbAttribute
    );
    return hr == S_OK;
}

//-----------------------------------------------------
// setWindowRoundedCorners()
//-----------------------------------------------------
static bool setWindowRoundedCorners(HWND hWnd, bool rounded)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr
            = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiLibrary.loadFunctionPointer("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr)
        return false;

    DWM_WINDOW_CORNER_PREFERENCE value = rounded ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,                           // hwnd
        DWMWA_WINDOW_CORNER_PREFERENCE, // dwAttribute
        &value,                         // pvAttribute
        sizeof(value)                   // cbAttribute
    );
    return hr == S_OK;
}

//-----------------------------------------------------
// setWindowCloaked()
//-----------------------------------------------------
static bool setWindowCloaked(HWND hWnd, bool cloaked)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr
            = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiLibrary.loadFunctionPointer("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr)
        return false;

    BOOL value = cloaked ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,         // hwnd
        DWMWA_CLOAK,  // dwAttribute
        &value,       // pvAttribute
        sizeof(value) // cbAttribute
    );
    return hr == S_OK;
}

//-----------------------------------------------------
// setWindowTitlebarTransparent()
//-----------------------------------------------------
static bool setWindowTitlebarTransparent(HWND hWnd, bool transparent)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr
            = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiLibrary.loadFunctionPointer("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr)
        return false;

    DWM_SYSTEMBACKDROP_TYPE value = transparent ? DWMSBT_TRANSIENTWINDOW : DWMSBT_MAINWINDOW;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,                      // hwnd
        DWMWA_SYSTEMBACKDROP_TYPE, // dwAttribute
        &value,                    // pvAttribute
        sizeof(value)              // cbAttribute
    );
    return hr == S_OK;
}

} // namespace windows
} // namespace cpputils

#endif
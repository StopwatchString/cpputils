#ifndef CPPUTILS_WINDOWS_DWM_H
#define CPPUTILS_WINDOWS_DWM_H

//-----------------------------------------------------
// dwm.h
// 
// A set of functions to handle interaction with the
// Desktop Window Mangement API for you.
//-----------------------------------------------------

#include <windows.h>
#include <dwmapi.h>

//-----------------------------------------------------
// PFN_DWM_SET_WINDOW_ATTRIBUTE
//-----------------------------------------------------
typedef HRESULT(WINAPI* PFN_DWM_SET_WINDOW_ATTRIBUTE)(
    HWND hwnd,
    DWORD dwAttribute,
    LPCVOID pvAttribute,
    DWORD cbAttribute
);
static PFN_DWM_SET_WINDOW_ATTRIBUTE DwmSetWindowAttributePtr = nullptr;

//-----------------------------------------------------
// class DWMAPIdllState
// 
// RAII management of loading dwmapi.dll at runtime.
// Users of these functions need not worry about
// calling FreeLibrary() themselves.
//-----------------------------------------------------
class DWMAPIdllState {
    const char* DWM_API_DLL_NAME {"dwmapi.dll"};
public:
    DWMAPIdllState() {
        hDWMAPIdll = LoadLibraryA(DWM_API_DLL_NAME);
    }
    ~DWMAPIdllState() {
        if (hDWMAPIdll != NULL) {
            FreeLibrary(hDWMAPIdll);
            hDWMAPIdll = NULL;
        }
    }

    void* loadDWMAPIProc(const char* procName) {
        return GetProcAddress(hDWMAPIdll, procName);
    }

private:
    HMODULE hDWMAPIdll           { NULL };
};
DWMAPIdllState dwmApiDllState;

//-----------------------------------------------------
// setWindowDarkMode()
//-----------------------------------------------------
static bool setWindowDarkMode(HWND hWnd, bool darkMode)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiDllState.loadDWMAPIProc("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr) return false;

    BOOL value = darkMode ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,                           // hwnd
        DWMWA_USE_IMMERSIVE_DARK_MODE,  // dwAttribute
        &value,                         // pvAttribute
        sizeof(value)                   // cbAttribute
    );
    return hr == S_OK;
}

//-----------------------------------------------------
// setWindowRoundedCorners()
//-----------------------------------------------------
static bool setWindowRoundedCorners(HWND hWnd, bool rounded)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiDllState.loadDWMAPIProc("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr) return false;

    DWM_WINDOW_CORNER_PREFERENCE value = rounded ? DWMWCP_ROUND : DWMWCP_DONOTROUND;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,                            // hwnd
        DWMWA_WINDOW_CORNER_PREFERENCE,  // dwAttribute
        &value,                          // pvAttribute
        sizeof(value)                    // cbAttribute
    );
    return hr == S_OK;
}

//-----------------------------------------------------
// setWindowCloaked()
//-----------------------------------------------------
static bool setWindowCloaked(HWND hWnd, bool cloaked)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiDllState.loadDWMAPIProc("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr) return false;

    BOOL value = cloaked ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,          // hwnd
        DWMWA_CLOAK,   // dwAttribute
        &value,        // pvAttribute
        sizeof(value)  // cbAttribute
    );
    return hr == S_OK;
}

//-----------------------------------------------------
// setWindowTitlebarTransparent()
//-----------------------------------------------------
static bool setWindowTitlebarTransparent(HWND hWnd, bool transparent)
{
    if (!DwmSetWindowAttributePtr) {
        DwmSetWindowAttributePtr = (PFN_DWM_SET_WINDOW_ATTRIBUTE)dwmApiDllState.loadDWMAPIProc("DwmSetWindowAttribute");
    }
    if (!DwmSetWindowAttributePtr) return false;

    DWM_SYSTEMBACKDROP_TYPE value = transparent ? DWMSBT_TRANSIENTWINDOW : DWMSBT_MAINWINDOW;
    HRESULT hr = DwmSetWindowAttributePtr(
        hWnd,                       // hwnd
        DWMWA_SYSTEMBACKDROP_TYPE,  // dwAttribute
        &value,                     // pvAttribute
        sizeof(value)               // cbAttribute
    );
    return hr == S_OK;
}

#endif
# cpputils

A collection of header-only implementation includes to help speed up development. The existing utilities mostly focus on OS interaction, but more general tools will eventually pop up.

## Table of Contents

[Interprocess Mutex](#cpputils/interprocesmutex)

[InterprocessMutexLockGuard](#cpputils/interpocessmutexlockguard)

## cpputils/InterprocessMutex.h

## cpputils/InterprocessMutexLockGuard.h

## cpputils/SharedLibraryLoader.h

## cpputils/SharedMemory.h

A header-only RAII implementation of an OS interprocess shared memory datatype. Uses a key and a 64bit uint to open a mapped piece of shared memory in either Windows or Linux. Provides a simple interface with just two getters:

```cpp
void data() { return pData; }
size_t size() const { return sizeBytes; }
```

## cpputils/windows/handle_utils.h

## cpputils/windows/dwm.h

An interface into the Desktop Window Management (DWM) library in windows. Handles importing DWM functions for you.

| Function                                                        | Utility                                                                                                                                                                       |
| --------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ```setWindowDarkMode(HWND hWnd, bool darkMode)```               | Sets a window's ```DWMWA_USE_IMMERSIVE_DARK_MODE``` flag to ```TRUE``` or ```FALSE```.                                                                                        |
| ```setWindowRoundedCorners(HWND hWnd, bool rounded)```          | Sets a window's ```DWMWA_WINDOW_CORNER_PREFERENCE``` attribute to either ```DWMWCP_ROUND``` or ```DWMWCP_DONOTROUND```                                                        |
| ```setWindowCloaked(HWND hWnd, bool cloaked)```                 | Sets a window's ```DWMWA_CLOAK``` attribute to ```TRUE``` or ```FALSE```. When a window is cloaked both the window and task bar icon for the window are hidden from the user. |
| ```setWindowTitlebarTransparent(HWND hWnd, bool transparent)``` | Sets a window's ```DWMWA_SYSTEMBACKDROP_TYPE``` attribute to ```DWMSBT_TRANSIENTWINDOW``` or ```DWMSBT_MAINWINDOW```.                                                         |

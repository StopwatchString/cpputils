# cpputils

A collection of header-only implementation includes to help speed up development. The existing utilities mostly focus on OS interaction, but more general tools will eventually pop up.

## Table of Contents

[Interprocess Mutex](#cpputils/interprocesmutexh)

[InterprocessMutexLockGuard](#cpputils/interpocessmutexlockguardh)

## cpputils/InterprocessMutex.h

## cpputils/InterprocessMutexLockGuard.h

## cpputils/SharedLibraryLoader.h

## cpputils/SharedMemory.h

A RAII implementation of an OS interprocess shared memory datatype. Uses a key and a 64bit uint to open a mapped piece of shared memory in either Windows or Linux. Interface:

| Function                                                 | Utility                                                                                                                                                      |
|----------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------|
| ```SharedMemory(const std::string& key, size_t bytes)``` | Constructor. Copy and move constructors disabled.                                                                                                            |
| ```void* data()```                                       | Returns a copy of the pointer to the underlying OS shared memory. If ```data() == nullptr```, then allocation of the OS shared memory has failed.            |
| ```size_t size()```                                      | Returns the requested size passed to the object on creation. Does not change, even if allocation failed. Nullptr check data() for validity checking instead. |

## cpputils/windows/handle_utils.h

## cpputils/windows/dwm.h

An interface into the Desktop Window Management (DWM) library in windows. Handles importing DWM functions for you.

| Function                                                        | Utility                                                                                                                                                                       |
| --------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ```setWindowDarkMode(HWND hWnd, bool darkMode)```               | Sets a window's ```DWMWA_USE_IMMERSIVE_DARK_MODE``` flag to ```TRUE``` or ```FALSE```.                                                                                        |
| ```setWindowRoundedCorners(HWND hWnd, bool rounded)```          | Sets a window's ```DWMWA_WINDOW_CORNER_PREFERENCE``` attribute to either ```DWMWCP_ROUND``` or ```DWMWCP_DONOTROUND```                                                        |
| ```setWindowCloaked(HWND hWnd, bool cloaked)```                 | Sets a window's ```DWMWA_CLOAK``` attribute to ```TRUE``` or ```FALSE```. When a window is cloaked both the window and task bar icon for the window are hidden from the user. |
| ```setWindowTitlebarTransparent(HWND hWnd, bool transparent)``` | Sets a window's ```DWMWA_SYSTEMBACKDROP_TYPE``` attribute to ```DWMSBT_TRANSIENTWINDOW``` or ```DWMSBT_MAINWINDOW```.                                                         |

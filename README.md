# cpputils

A collection of header-only implementation includes to help speed up development. The existing utilities mostly focus on OS interaction, but more general tools will eventually pop up.

## Table of Contents

[Interprocess Mutex](#cpputils/interprocesmutex)

## cpputils/InterprocessMutex

## cpputils/InterprocessMutexLockGuard

## cpputils/SharedLibraryLoader

## cpputils/SharedMemory

A header-only RAII implementation of an OS interprocess shared memory datatype. Uses a key and a 64bit uint to open a mapped piece of shared memory in either Windows or Linux. Provides a simple interface with just two getters:

```cpp
void data() { return pData; }
size_t size() const { return sizeBytes; }
```

## cpputils/windows/handle_utils

## cpputils/windows/dwm

### Depends on cpputils/SharedLibraryLoader

A simple interface into the Desktop Window Management (DWM) library in windows. Handles importing DWM functions for you.

Current functions:

- setWindowDarkMode()
- setWindowRoundedCorners()
- setWindowCloaked() (cloaked is defined as existing but invisible to the user- ie, not visible, and not in taskbar)
- setWindowTitlebarTransparent()

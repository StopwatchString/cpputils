# cpputils

## SharedMemory

A header-only RAII implementation of an OS interprocess shared memory datatype. Uses a key and a 64bit uint to open a mapped piece of shared memory in either Windows or Linux. Provides a simple interface with just two getters:

```cpp
void data() { return pData; }
size_t size() const { return sizeBytes; }
```
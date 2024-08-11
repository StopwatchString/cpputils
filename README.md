# cpputils

## SharedMemory
A header-only implementation of a managed shared memory type. Supports either typed usage to autoallocate the correct size or untyped usage with manual size input.

Currently only windows implementation.

- Untyped
```
std::string key = "shmKey";
uint32_t sizeInBytes = 128;

SharedMemory<> shm(key, sizeInBytes);
```

- Typed
```
std::string key = "shmKey";
SharedMemory<Type> shm;

```
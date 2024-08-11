# cpputils

## SharedMemory
A header-only implementation of a managed shared memory type. Supports either typed usage to autoallocate the correct size or untyped usage with manual size input.

Currently only windows implementation.

- Untyped
```
    std::string key = "shmKey";
    uint32_t size = 128;
    SharedMemory<> shm(key, size);
    
    void* data = shm.getData();     // Pointer to shared memory data block
    uint32_t dataSize = shm.size(); // dataSize == 128
```

- Typed
```
    struct Pos {
        uint32_t x = 0;
        uint32_t y = 0;
        uint32_t z = 0;
    };

    std::string key = "shmKey";
    SharedMemory<Pos> shm(key);
    
    Pos& data = *shm.getData();     // Pointer returned is pre-typed to the template type
    uint32_t dataSize = shm.size(); // dataSize == sizeof(Pos)

```
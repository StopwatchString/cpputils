# cpputils

## SharedMemory

A header-only SharedMemory wrapper. Acquires SharedMemory handle from OS upon construction and closes SharedMemory handle upon destruction.

Two main modes:

- Untyped (default/void template)
  - Constructor supports arbitrary size
  - Direct access to raw memory
- Typed
  - Automatically allocates only enough space for the template type
  - Supports implicit casting to template type
  - operator-> overloaded for direct member access on types with members
  - Still supports direct access to raw memory

TODO

- Linux Implementation
- Extra functionality baked into SharedMemory allocation
  - refcounting: lookup number of SharedMemory handles opened on the current key
  - NamedMutex: Implement Windows/Linux interprocess mutex class then use it to allow for lock()/unlock()/trylock() impl

Currently only implemented for Windows, Linux support soon.

### Untyped

```cpp
std::string key = "mem";
uint32_t size = 128;
SharedMemory<> shm(key, size);

void* data = shm.data();         // Pointer to shared memory data block
uint32_t dataSize = shm.size();  // dataSize == 128
std::string dataKey = shm.key(); // dataKey == "mem"
```

### Typed

```cpp
std::string key = "mem";
uint32_t size = 128;
SharedMemory<uint32_t> shm(key);

uint32_t* data = shm.data();     // Pointer to shared memory data block
uint32_t dataSize = shm.size();  // dataSize == sizeof(uint32_t)
std::string dataKey = shm.key(); // dataKey == "mem"
```

### Usage examples

```cpp
struct Pos {
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

SharedMemory<uint32_t> shm1("mem1");
SharedMemory<uint32_t> shm2("mem1"); // shm1 and shm2 now refer to the same memory.
SharedMemory<Pos>      shm3("mem3");
SharedMemory<>         shm4("mem4", 16);

// Check if SharedMemory opened by comparing data pointer with nullptr
if (shm4.data() == nullptr) {
    std::cout << "Shared memory isn't open!" << std::endl;
}

// Assignment matching template type are overloaded to be passed through to the underlying datatype
shm1 = 2;
shm3 = { 2, 3, 4 };

// Implicit casting to underlying datatype is supported.
int b = 3 + shm2;
std::cout << shm2 << std::endl;

// Arrow operator has been overloaded to allow direct access to underlying datatype (provided it is a type with members)
// Not sure yet if this is good design... but it's neat you can do it!
std::cout << shm3->x << std::endl;
```

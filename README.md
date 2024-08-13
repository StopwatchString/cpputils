# cpputils

## SharedMemory
A header-only implementation of a managed shared memory type. Supports either typed usage to autoallocate the correct size or untyped usage with manual size input.

Currently only windows implementation.

### Untyped
```cpp
std::string key = "mem";
uint32_t size = 128;
SharedMemory<> shm(key, size);   // SharedMemory closes when it goes out of scope

void* data = shm.data();         // Pointer to shared memory data block
uint32_t dataSize = shm.size();  // dataSize == 128
std::string dataKey = shm.key(); // dataKey == "mem"
```

### Typed
```cpp
std::string key = "mem";
uint32_t size = 128;
SharedMemory<uint32_t> shm(key); // SharedMemory closes when it goes out of scope

uint32_t* data = shm.data();     // Pointer to shared memory data block
uint32_t dataSize = shm.size();  // dataSize == sizeof(uint32_t)
std::string dataKey = shm.key(); // dataKey == "mem"
```

### Untyped usage examples
```cpp
struct Pos {
    uint32_t x;
    uint32_t y;
    uint32_t z;
};

SharedMemory<uint32_t> shm("mem");
SharedMemory<uint32_t> shm2("mem2");
SharedMemory<Pos>      shm3("mem3");

// Assignments are overloaded and passed through to the underlying datatype
shm = 2;
shm3 = { 2, 3, 4 };

// Arithmetic operators are overloaded to return the result of the underlying datatype.
uint32_t data = shm + shm2;

// Implicit casting to underlying datatype is also supported.
int a = 3;
int b = a + shm2;

// Implicit casting means that ostream operator works too!
std::cout << shm << std::endl;

// Arrow operator has been overloaded to allow direct access to underlying datatype (provided it is a type with members)
// Not sure yet if this is good design... but it's neat you can do it!
std::cout << shm3->x << std::endl; // Prints out '2'
```

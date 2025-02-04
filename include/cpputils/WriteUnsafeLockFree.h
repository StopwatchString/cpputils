#ifndef CPPUTILS_WRITE_UNSAFE_LOCK_FREE_H
#define CPPUTILS_WRITE_UNSAFE_LOCK_FREE_H

#include <array>

namespace cpputils {

//------------------------------------------------------------
// class WriteUnsafeLockFree
//------------------------------------------------------------
template<typename T>
class WriteUnsafeLockFree
{
    static constexpr size_t BUFFER_SIZE = 3;

public:
    //------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree() : readIndex(0) {}

    //------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree(const T& data) : readIndex(0)
    {
        ringBuffer[0] = data;
    }

    //------------------------------------------------------------
    // Destructor
    //------------------------------------------------------------
    ~WriteUnsafeLockFree() {}

    //------------------------------------------------------------
    // Copy Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree(const WriteUnsafeLockFree<T>& other) : ringBuffer(other.ringBuffer), readIndex(other.readIndex)
    {}

    //------------------------------------------------------------
    // Copy Assignment
    //------------------------------------------------------------
    WriteUnsafeLockFree<T>& operator=(const WriteUnsafeLockFree<T>& other)
    {
        ringBuffer = other.ringBuffer;
        readIndex = other.readIndex;
    }

    //------------------------------------------------------------
    // Move Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree(WriteUnsafeLockFree<T>&& other) noexcept :
        ringBuffer(std::move(other.ringBuffer)), readIndex(other.readIndex)
    {
        other.ringBuffer = {};
        other.readIndex = 0;
    }

    //------------------------------------------------------------
    // Move Assignment
    //------------------------------------------------------------
    WriteUnsafeLockFree<T>& operator=(WriteUnsafeLockFree<T>&& other) noexcept
    {
        if (this != &other) {
            ringBuffer = std::move(other.ringBuffer);
            readIndex = other.readIndex;
        }
    }

    //------------------------------------------------------------
    // get()
    //------------------------------------------------------------
    const T& get() const
    {
        return ringBuffer[readIndex];
    }

    //------------------------------------------------------------
    // set()
    //------------------------------------------------------------
    void set(const T& data)
    {
        ringBuffer[(readIndex + 1) % BUFFER_SIZE] = data;
        readIndex = (readIndex + 1) % BUFFER_SIZE;
    }

private:
    std::array<T, BUFFER_SIZE> ringBuffer{};
    size_t readIndex{0};
};

} // namespace cpputils

#endif
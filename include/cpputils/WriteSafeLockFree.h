#include <mutex>

//------------------------------------------------------------
// class WriteSafeLockFree
//------------------------------------------------------------
template <typename T, size_t BUFFER_SIZE>
class WriteSafeLockFree
{
public:
    //------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------
    WriteSafeLockFree()
        : readIndex(0)
    {

    }

    //------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------
    WriteSafeLockFree(const T& data)
        : readIndex(0)
    {
        ringBuffer[0] = data;
    }

    //------------------------------------------------------------
    // Destructor
    //------------------------------------------------------------
    ~WriteSafeLockFree()
    {
        // TODO
    }

    //------------------------------------------------------------
    // Copy Constructor
    //------------------------------------------------------------
    WriteSafeLockFree(const WriteSafeLockFree<T, BUFFER_SIZE>& other)
    {
        // TODO
    }

    //------------------------------------------------------------
    // Copy Assignment
    //------------------------------------------------------------
    WriteSafeLockFree<T, BUFFER_SIZE>& operator=(const WriteSafeLockFree<T, BUFFER_SIZE>& other)
    {
        // TODO
    }

    //------------------------------------------------------------
    // Move Constructor
    //------------------------------------------------------------
    WriteSafeLockFree(WriteSafeLockFree<T, BUFFER_SIZE>&& other) noexcept
    {
        // TODO
    }

    //------------------------------------------------------------
    // Move Assignment
    //------------------------------------------------------------
    WriteSafeLockFree<T, BUFFER_SIZE>& operator=(WriteSafeLockFree<T, BUFFER_SIZE>&& other) noexcept
    {
        // TODO
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
        std::lock_guard<std::mutex> lock(write);
        ringBuffer[(readIndex + 1) % BUFFER_SIZE] = data;
        readIndex = (readIndex + 1) % BUFFER_SIZE;
    }

private:
    T ringBuffer[BUFFER_SIZE]{};
    size_t readIndex{ 0 };
    std::mutex write;
};
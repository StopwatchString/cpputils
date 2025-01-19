#include <array>
#include <mutex>

//------------------------------------------------------------
// class WriteSafeLockFree
//------------------------------------------------------------
template <typename T>
class WriteSafeLockFree
{
    static constexpr size_t BUFFER_SIZE = 3;
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
    WriteSafeLockFree(const WriteSafeLockFree<T>& other)
    {
        // TODO
    }

    //------------------------------------------------------------
    // Copy Assignment
    //------------------------------------------------------------
    WriteSafeLockFree<T>& operator=(const WriteSafeLockFree<T>& other)
    {
        // TODO
    }

    //------------------------------------------------------------
    // Move Constructor
    //------------------------------------------------------------
    WriteSafeLockFree(WriteSafeLockFree<T>&& other) noexcept
    {
        // TODO
    }

    //------------------------------------------------------------
    // Move Assignment
    //------------------------------------------------------------
    WriteSafeLockFree<T>& operator=(WriteSafeLockFree<T>&& other) noexcept
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

    template <typename U>
    void set(const U& data) = delete;

private:
    std::array<T, BUFFER_SIZE> ringBuffer{};
    size_t readIndex{ 0 };
    std::mutex write;
};
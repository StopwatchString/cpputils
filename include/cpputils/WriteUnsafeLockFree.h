//------------------------------------------------------------
// class WriteUnsafeLockFree
//------------------------------------------------------------
template <typename T, size_t BUFFER_SIZE>
class WriteUnsafeLockFree
{
public:
    //------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree()
        : readIndex(0)
    {

    }

    //------------------------------------------------------------
    // Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree(const T& data)
        : readIndex(0)
    {
        ringBuffer[0] = data;
    }

    //------------------------------------------------------------
    // Destructor
    //------------------------------------------------------------
    ~WriteUnsafeLockFree()
    {
        // TODO
    }

    //------------------------------------------------------------
    // Copy Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree(const WriteUnsafeLockFree<T, BUFFER_SIZE>& other)
    {
        // TODO
    }

    //------------------------------------------------------------
    // Copy Assignment
    //------------------------------------------------------------
    WriteUnsafeLockFree<T, BUFFER_SIZE>& operator=(const WriteUnsafeLockFree<T, BUFFER_SIZE>& other)
    {
        // TODO
    }

    //------------------------------------------------------------
    // Move Constructor
    //------------------------------------------------------------
    WriteUnsafeLockFree(WriteUnsafeLockFree<T, BUFFER_SIZE>&& other) noexcept
    {
        // TODO
    }

    //------------------------------------------------------------
    // Move Assignment
    //------------------------------------------------------------
    WriteUnsafeLockFree<T, BUFFER_SIZE>& operator=(WriteUnsafeLockFree<T, BUFFER_SIZE>&& other) noexcept
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
        ringBuffer[(readIndex + 1) % BUFFER_SIZE] = data;
        readIndex = (readIndex + 1) % BUFFER_SIZE;
    }

private:
    T ringBuffer[BUFFER_SIZE]{};
    size_t readIndex{ 0 };
};
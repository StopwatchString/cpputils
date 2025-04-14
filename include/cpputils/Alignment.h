#ifndef CPPUTILS_ALIGNED_BUFFER_H
#define CPPUTILS_ALIGNED_BUFFER_H


#ifdef _WIN32
    #include <malloc.h>

    #define ALIGNED_ALLOC(size, alignment) _aligned_malloc(size, alignment)
    #define ALIGNED_FREE(ptr) _aligned_free(ptr)
#elif defined(__linux__)
    #define ALIGNED_ALLOC(size, alignment) std::aligned_alloc(alignment, size)
    #define ALIGNED_FREE(ptr) std::free(ptr)
#endif

namespace cpputils {
    void* aligned_alloc(size_t size, size_t alignment)
    {
        return ALIGNED_ALLOC(size, alignment);
    }

    void aligned_free(void* ptr)
    {
        ALIGNED_FREE(ptr);
    }

struct AlignedBuffer {
    void* buf{ nullptr };
    size_t size{ 0 };
    size_t alignment{ 0 };

    AlignedBuffer() = default;
    AlignedBuffer(size_t inSize, size_t inAlignment)
        : size(inSize),
        alignment(inAlignment)
    {
        buf = aligned_alloc(size, alignment);
        if (buf == nullptr) {
            size = 0;
            alignment = 0;
        }
    }

    ~AlignedBuffer()
    {
        aligned_free(buf);
    }

    AlignedBuffer(const AlignedBuffer& other) = delete;
    AlignedBuffer& operator=(const AlignedBuffer& other) = delete;

    AlignedBuffer(AlignedBuffer&& other) noexcept
        : buf(other.buf),
        size(other.size),
        alignment(other.alignment)
    {
        other.buf = nullptr;
        other.size = 0;
        other.alignment = 0;
    }
    AlignedBuffer& operator=(AlignedBuffer&& other) noexcept
    {
        if (this != &other) {
            aligned_free(buf);
            buf = other.buf;
            size = other.size;
            alignment = other.alignment;

            other.buf = nullptr;
            other.size = 0;
            other.alignment = 0;
        }
        return *this;
    }
};
} // End cpputils

#endif

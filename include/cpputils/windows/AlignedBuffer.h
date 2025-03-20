#ifndef CPPUTILS_WINDOWS_ALIGNED_BUFFER_H
#define CPPUTILS_WINDOWS_ALIGNED_BUFFER_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace cpputils {
namespace win {
    struct AlignedBuffer {
        void* buf{ nullptr };
        size_t size{ 0 };
        size_t alignment{ 0 };

        AlignedBuffer() = default;
        AlignedBuffer(size_t inSize, size_t inAlignment)
            : size(inSize),
            alignment(inAlignment)
        {
            buf = _aligned_malloc(size, alignment);
            if (buf == nullptr) {
                size = 0;
                alignment = 0;
            }
        }

        ~AlignedBuffer()
        {
            _aligned_free(buf);
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
                _aligned_free(buf);
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
} // End win
} // End cpputils

#endif

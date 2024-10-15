#ifndef STREAM_TEXTURE_H
#define STREAM_TEXTURE_H

#include <cstdint>

class StreamTexture
{
public:
    StreamTexture(uint32_t width, uint32_t height, uint32_t depth);
    ~StreamTexture();

    void init();
    void bind();
    uint32_t getBufferSize();
    void copyDataInto(void* data);

private:
    uint32_t m_Width{ 0 };
    uint32_t m_Height{ 0 };
    uint32_t m_Depth{ 0 };

    // Associated OpenGL context reference? Win32 parameters?

    //GLuint m_Texture;
    //GLuint m_Pbo;
    void* pboPersistentlyMappedMemory{ nullptr };
};

#endif
#ifndef STREAM_TEXTURE_H
#define STREAM_TEXTURE_H

#include <cstdint>
#include <string>

class StreamTexture
{
public:
    StreamTexture(std::string name, uint32_t width, uint32_t height, uint32_t depth);
    ~StreamTexture();

    void init();
    void bind();
    uint32_t getBufferSize();
    std::string getName() const { return m_Name; }
    void copyDataInto(void* inData, uint32_t inDataSize);

private:
    std::string m_Name;
    uint32_t m_Width{ 0 };
    uint32_t m_Height{ 0 };
    uint32_t m_Depth{ 0 };

    // Associated OpenGL context reference? Win32 parameters?

    //GLuint m_Texture;
    //GLuint m_Pbo;
    void* pboPersistentlyMappedMemory{ nullptr };
};

#endif
#include "StreamTexture.h"

StreamTexture::StreamTexture(uint32_t width, uint32_t height, uint32_t depth)
    : m_Width(width),
    m_Height(height),
    m_Depth(depth),
    pboPersistentlyMappedMemory(nullptr)
{

}

StreamTexture::~StreamTexture()
{
    // Cleanup graphics resources, needs to visit the context its associated with?
}

void StreamTexture::init()
{
    // should be called within the relevant graphics context
}

void StreamTexture::bind()
{
    // glBindTexture(m_Texture);
}

uint32_t StreamTexture::getBufferSize()
{
    return m_Width * m_Height * m_Depth;
}

void StreamTexture::copyDataInto(void* data)
{
    if (pboPersistentlyMappedMemory != nullptr) {

    }
}
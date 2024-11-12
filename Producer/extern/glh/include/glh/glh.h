#ifndef GLH_H
#define GLH_H

#include "../glad/gl.h"

#ifdef _WIN32
#include "../glad/wgl.h"
#elif defined(__linux__)
#include "../glad/glx.h"
#endif

#include "glh/debugWrappers.h"

#include <iostream>
#include <fstream>
#include <vector>

//-----------------------------------------------
// glhLoadFile()
//-----------------------------------------------
static const char* glhLoadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return nullptr; // File couldn't be opened
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size + 1);
    if (!file.read(buffer.data(), size)) {
        return nullptr; // Read failed
    }

    buffer[size] = '\0'; // Null-terminate the buffer

    char* result = new char[size + 1];
    std::copy(buffer.begin(), buffer.end(), result);

    return result;
}

//-----------------------------------------------
// glhErrorCheck()
//-----------------------------------------------
static void glhErrorCheck(const std::string& location) {
    GLenum errorCode = glGetError();
    if (errorCode != GL_NO_ERROR) {
        switch (errorCode) {
            case GL_INVALID_ENUM: {
                std::cerr << location << " " << "ErrorCode GL_INVALID_ENUM (" << GL_INVALID_ENUM << ")" << '\n';
                break;
            }
            case GL_INVALID_VALUE: {
                std::cerr << location << " " << "ErrorCode GL_INVALID_VALUE (" << GL_INVALID_VALUE << ")" << '\n';
                break;
            }
            case GL_INVALID_OPERATION: {
                std::cerr << location << " " << "ErrorCode GL_INVALID_OPERATION (" << GL_INVALID_OPERATION << ")" << '\n';
                break;
            }
            case GL_STACK_OVERFLOW: {
                std::cerr << location << " " << "ErrorCode GL_STACK_OVERFLOW (" << GL_STACK_OVERFLOW << ")" << '\n';
                break;
            }
            case GL_STACK_UNDERFLOW: {
                std::cerr << location << " " << "ErrorCode GL_STACK_UNDERFLOW (" << GL_STACK_UNDERFLOW << ")" << '\n';
                break;
            }
            case GL_OUT_OF_MEMORY: {
                std::cerr << location << " " << "ErrorCode GL_OUT_OF_MEMORY (" << GL_OUT_OF_MEMORY << ")" << '\n';
                break;
            }
            case GL_INVALID_FRAMEBUFFER_OPERATION: {
                std::cerr << location << " " << "ErrorCode GL_INVALID_FRAMEBUFFER_OPERATION (" << GL_INVALID_FRAMEBUFFER_OPERATION << ")" << '\n';
                break;
            }
            case GL_CONTEXT_LOST: {
                std::cerr << location << " " << "ErrorCode GL_CONTEXT_LOST (" << GL_CONTEXT_LOST << ")" << '\n';
                break;
            }
            case GL_TABLE_TOO_LARGE: {
                std::cerr << location << " " << "ErrorCode GL_TABLE_TOO_LARGE (" << GL_TABLE_TOO_LARGE << ")" << '\n';
                break;
            }
            default: {
                std::cerr << location << " " << "Unknown ErrorCode (" << errorCode << ")" << '\n';
                break;
            }
        }
    }
}

//-----------------------------------------------
// glhLoadExtensions()
//-----------------------------------------------
static bool glhLoadExtensions(GLADloadfunc load) {
    return gladLoadGL((GLADloadfunc)load);
}

//-----------------------------------------------
// glhLoadPlatformExtensions()
//-----------------------------------------------
#ifdef _WIN32
static bool glhLoadPlatformExtensions(HDC hdc, GLADloadfunc load) {
    return gladLoadWGL(hdc, (GLADloadfunc)load);
}
#elif defined(__linux__)
// TODO:: Linux Implementation
#endif

#endif
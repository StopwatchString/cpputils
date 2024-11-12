#ifndef GLH_MACROS_H
#define GLH_MACROS_H

#ifndef NDEBUG
#include <iostream>

#define GL_ERROR_CHECK(glFuncName)                                                                                                                     \
do {                                                                                                                                                   \
    GLenum errorCode = glGetError();                                                                                                                   \
    if (errorCode != GL_NO_ERROR) {                                                                                                                    \
        switch (errorCode) {                                                                                                                           \
            case GL_INVALID_ENUM: {                                                                                                                    \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_INVALID_ENUM (" << GL_INVALID_ENUM << ")" << '\n';                                    \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_INVALID_VALUE: {                                                                                                                   \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_INVALID_VALUE (" << GL_INVALID_VALUE << ")" << '\n';                                  \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_INVALID_OPERATION: {                                                                                                               \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_INVALID_OPERATION (" << GL_INVALID_OPERATION << ")" << '\n';                          \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_STACK_OVERFLOW: {                                                                                                                  \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_STACK_OVERFLOW (" << GL_STACK_OVERFLOW << ")" << '\n';                                \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_STACK_UNDERFLOW: {                                                                                                                 \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_STACK_UNDERFLOW (" << GL_STACK_UNDERFLOW << ")" << '\n';                              \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_OUT_OF_MEMORY: {                                                                                                                   \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_OUT_OF_MEMORY (" << GL_OUT_OF_MEMORY << ")" << '\n';                                  \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_INVALID_FRAMEBUFFER_OPERATION: {                                                                                                   \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_INVALID_FRAMEBUFFER_OPERATION (" << GL_INVALID_FRAMEBUFFER_OPERATION << ")" << '\n';  \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_CONTEXT_LOST: {                                                                                                                    \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_CONTEXT_LOST (" << GL_CONTEXT_LOST << ")" << '\n';                                    \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            case GL_TABLE_TOO_LARGE: {                                                                                                                 \
                std::cerr <<  glFuncName << " " << "ErrorCode GL_TABLE_TOO_LARGE (" << GL_TABLE_TOO_LARGE << ")" << '\n';                              \
                break;                                                                                                                                 \
            }                                                                                                                                          \
            default: {                                                                                                                                 \
                std::cerr <<  glFuncName << " " << "Unknown ErrorCode (" << errorCode << ")" << '\n';                                                  \
                break;                                                                                                                                 \
            }                                                                                                                                          \
        }                                                                                                                                              \
    }                                                                                                                                                  \
} while (0)

#else

#define GL_ERROR_CHECK(glFuncName)

#endif


#endif
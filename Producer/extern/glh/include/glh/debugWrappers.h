#ifndef GLH_DEBUG_WRAPPERS_H
#define GLH_DEBUG_WRAPPERS_H

#include "glad/gl.h"
#include "glh/macros.h"

// Framebuffers

// Textures

static void glhGenTextures(GLsizei num, GLuint* textures) {
    GL_ERROR_CHECK("Before glGenTextures()");
    glGenTextures(num, textures);
    GL_ERROR_CHECK("glGenTextures()");
}

// Clearing
static void glhClear(GLenum clearTarget) {
    GL_ERROR_CHECK("Before glClear");
    glClear(clearTarget);
    GL_ERROR_CHECK("After glClear");
}

static void glhClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
    GL_ERROR_CHECK("Before glClearColor");
    glClearColor(r, g, b, a);
    GL_ERROR_CHECK("After glClearColor");
}

#endif
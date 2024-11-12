#include "OpenGLApplication.h"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "glh/glh.h"

#include "directx_utils.h"
#include "cpputils/windows/handle_utils.h"

#include <iostream>
#include <array>

OpenGLApplication::ApplicationConfig appConfig{};

constexpr int moveFactor = 100;
void keyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(glfwWindow, GLFW_TRUE);
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        int xPos, yPos;
        glfwGetWindowPos(glfwWindow, &xPos, &yPos);
        glfwSetWindowPos(glfwWindow, xPos - moveFactor, yPos);
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        int xPos, yPos;
        glfwGetWindowPos(glfwWindow, &xPos, &yPos);
        glfwSetWindowPos(glfwWindow, xPos + moveFactor, yPos);
    }

    if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
        int xPos, yPos;
        glfwGetWindowPos(glfwWindow, &xPos, &yPos);
        glfwSetWindowPos(glfwWindow, xPos, yPos - moveFactor);
    }

    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        int xPos, yPos;
        glfwGetWindowPos(glfwWindow, &xPos, &yPos);
        glfwSetWindowPos(glfwWindow, xPos, yPos + moveFactor);
    }
}

void draw(GLFWwindow* window) {
    glfwMakeContextCurrent(window);

    // Create D3D resources for a texture that can be shared across processes
    D3DState d3dState = initDirect3D();
    SharedTexture sharedTexture = createSharedTexture(d3dState, 800, 800, false, false);
    glhErrorCheck("DirectX Init");
    EmittedHandle e("key", sharedTexture.hSharedTexture);

    // Create an alternate framebuffer to render to
    GLuint framebuffer = 0;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Create a texture
    GLuint texture;
    glhGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 200, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    wglDXLockObjectsNV(d3dState.hWglD3DDevice, 1, &sharedTexture.hSharedTextureLock);

    // Attach the texture to the framebuffer, such that writing to the framebuffer will write to the texture
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sharedTexture.glTextureShared, 0);
    glhErrorCheck("glFramebufferTexture2D");

    wglDXUnlockObjectsNV(d3dState.hWglD3DDevice, 1, &sharedTexture.hSharedTextureLock);

    // Check that the framebuffer is complete (ready to be used)
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer is not complete!");
    }

    glEnable(GL_TEXTURE_2D);

    while (!glfwWindowShouldClose(window)) {
        glhErrorCheck("Start of Render");
        // Render to the framebuffer/texture
        //glBindTexture(GL_TEXTURE_2D, sharedTexture.glTextureShared);
        wglDXLockObjectsNV(d3dState.hWglD3DDevice, 1, &sharedTexture.hSharedTextureLock);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glhClearColor(0.0, 0.0, sin(glfwGetTime()) * 0.5 + 0.5, 1.0);
        glhClear(GL_COLOR_BUFFER_BIT);

        // Render to the screen framebuffer, drawing a triangle that uses the texture from the framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glhClearColor(1.0, 0.0, 0.0, 1.0);
        glhClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, sharedTexture.glTextureShared);
        glBegin(GL_TRIANGLES);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(0.0f, 1.0f);

        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(1.0f, 0.0f);

        glEnd();

        wglDXUnlockObjectsNV(d3dState.hWglD3DDevice, 1, &sharedTexture.hSharedTextureLock);
        //glBindTexture(GL_TEXTURE_2D, sharedTexture.glTextureShared);
        //glBegin(GL_TRIANGLES);

        //glTexCoord2f(0.0f, 0.0f);
        //glVertex2f(-0.5f, -0.5f);

        //glTexCoord2f(0.0f, 0.5f);
        //glVertex2f(0.0f, 0.5f);

        //glTexCoord2f(0.5f, 0.0f);
        //glVertex2f(0.5f, 0.0f);

        //glEnd();


        glhErrorCheck("End of Render");
        glfwSwapBuffers(window);
    }
}

int main(int argc, char argv[])
{
    appConfig.windowName = "Producer";
    appConfig.windowInitWidth = 1000;
    appConfig.windowInitHeight = 1000;
    appConfig.windowPosX = 4120;
    appConfig.windowPosY = 0;
    appConfig.windowBorderless = false;
    appConfig.windowResizeEnable = false;
    appConfig.glVersionMajor = 4;
    appConfig.glVersionMinor = 6;
    appConfig.customDrawFunc = &draw;
    appConfig.customKeyCallback = &keyCallback;

    try {
        OpenGLApplication application(appConfig);
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
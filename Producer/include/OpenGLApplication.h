#ifndef OPENGL_APPLICATION_H
#define OPENGL_APPLICATION_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "glh/glh.h"

#include <thread>
#include <string>
#include <functional>

class OpenGLApplication
{
public:
    struct ApplicationConfig
    {
        std::string windowName;
        int windowInitWidth      { 0 };
        int windowInitHeight     { 0 };
        int windowPosX           { 0 };
        int windowPosY           { 0 };
        bool windowBorderless    { false };
        bool windowResizeEnable  { false };
        int glVersionMajor       { 4 };
        int glVersionMinor       { 6 };
        std::function<void(GLFWwindow*)> customDrawFunc = nullptr;
        GLFWkeyfun customKeyCallback = nullptr;
        GLFWerrorfun customErrorCallback = nullptr;
    };

    OpenGLApplication(const ApplicationConfig& appConfig);
    ~OpenGLApplication();

private:
    void initGLFW();
    void initExtensions();
    void startRenderThread();
    void eventLoop();

    void renderFunc();

    ApplicationConfig appConfig{};

    GLFWwindow* glfwWindow{ nullptr };
    std::thread renderThread;

};

#endif
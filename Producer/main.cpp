
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"

#include "glh/glh.h"

#include <iostream>

#include <cmath>

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void glfwErrorCallback(int errorcode, const char* description)
{
    std::cerr << "GLFW Error! Code: " << errorcode << " Description: " << description << std::endl;
}

double scale(double oldLower, double oldUpper, double newLower, double newUpper, double input) {
    input -= oldLower;               // Move old lower bound to 0
    input /= (oldUpper - oldLower);  // Normalize value between 0 and 1
    input *= (newUpper - newLower);  // Scale normalized value to new bound size
    input += newLower;               // Move scale to new lower bound
    return input;
}

int main(int argc, char argv[])
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed!" << std::endl;
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_VERSION_MINOR, 6);

    GLFWwindow* window = glfwCreateWindow(800, 600, "grass-render", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        return EXIT_FAILURE;
    }

    glfwSetKeyCallback(window, glfwKeyCallback);
    glfwSetErrorCallback(glfwErrorCallback);

    glfwMakeContextCurrent(window);

    if (!glh::utils::loadExtensions(glfwGetProcAddress)) {
        std::cerr << "Failed to load OpenGL extensions!";
        return EXIT_FAILURE;
    }


    while (glfwWindowShouldClose(window) != GLFW_TRUE) {
        glfwPollEvents();
        glh::utils::glErrorCheck("Start of render loop");

        double r = 0.0f;
        double g = 0.0f;
        double b = scale(-1.0, 1.0, 0.0, 1.0, sin(glfwGetTime()));
        double a = 0.0f;
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);

        glh::utils::glErrorCheck("End of render loop");
        glfwSwapBuffers(window);
    }
}
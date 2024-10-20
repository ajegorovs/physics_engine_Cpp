#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>     // GLFWwindow


class VApp {
public:
    void run();
    bool framebufferResized = false; 
    GLFWwindow* window;
private:
    void initWindow();
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
};

#pragma once
//#include "config.h"

#ifndef BASEENVIRONMENT_H
#define BASEENVIRONMENT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class BaseEnvironment {
private:

public:
    bool framebufferResized = false;
    const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    int k = 10;
    GLFWwindow* window;
    VkInstance instance;
    VkSurfaceKHR surface;
    BaseEnvironment() {};

    void cleanup();
};
#endif
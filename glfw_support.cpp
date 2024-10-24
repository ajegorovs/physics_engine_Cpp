#include "glfw_support.h"
#include "config.h"

GLFW_support::GLFW_support()
{
    std::cout <<"GLFW_support: window <-" << window;
}

void GLFW_support::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto app = reinterpret_cast<GLFW_support*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}


// pre-vulkan init
void GLFW_support::initWindow() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}


std::vector<const char*> GLFW_support::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void GLFW_support::createSurface(VkInstance instance, VkSurfaceKHR* surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

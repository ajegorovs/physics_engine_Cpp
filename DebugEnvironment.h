#pragma once
//#include "config.h"
//#include "BaseEnvironment.h"

#ifndef DEBUGENVIRONMENT_H
#define DEBUGENVIRONMENT_H
#include <memory>
#include <iostream>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "BaseEnvironment.h"
#include "Timer.h"
#include "config.h"

class DebugEnvironment {
private:
    std::shared_ptr<BaseEnvironment> base; // Shared pointer to the base environment
    VkDebugUtilsMessengerEXT debugMessenger;
    Timer timer;
public:

    DebugEnvironment(std::shared_ptr<BaseEnvironment> env) : base(env) {}
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void cleanup(std::shared_ptr<BaseEnvironment> env);
    void setupDebugMessenger();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    bool checkValidationLayerSupport();

};
#endif

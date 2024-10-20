#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily; // strange that i dont have to include #include <cstdint>
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VDevice {
public:
    /*QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    void pickPhysicalDevice();
    void createLogicalDevice();
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);*/
    std::vector<const char*> getRequiredExtensions();
    VkDevice device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    VkQueue graphicsQueue;
    const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};


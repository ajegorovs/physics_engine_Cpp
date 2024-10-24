#pragma once
//#include "config.h"

#ifndef DEVICE_H
#define DEVICE_H

#include <memory>
#include <set>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "BaseEnvironment.h"
#include "structs.h"
#include "Timer.h"
#include "config.h"

class Device {
private:
    std::shared_ptr<BaseEnvironment> base; 
    Timer timer;
public:
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkDevice device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    Device(std::shared_ptr<BaseEnvironment> env) : base(env) {}

    void cleanup();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

    bool isDeviceSuitable(VkPhysicalDevice device);

    VkSampleCountFlagBits getMaxUsableSampleCount();

    void pickPhysicalDevice();

    void createLogicalDevice();

    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

    VkFormat findDepthFormat();

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
};
#endif

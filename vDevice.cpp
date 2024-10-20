#pragma once
#include "vDevice.h"
#include "config.h"
#include <stdexcept>
#include <vector>
#include <set>
#include <GLFW/glfw3.h>
#include <iostream>




std::vector<const char*> VDevice::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}
//
//uint32_t VDevice::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//    VkPhysicalDeviceMemoryProperties memProperties;
//    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//            return i;
//        }
//    }
//
//    throw std::runtime_error("failed to find suitable memory type!");
//}
//
//QueueFamilyIndices VDevice::findQueueFamilies(VkPhysicalDevice device) {
//    QueueFamilyIndices indices;
//
//    uint32_t queueFamilyCount = 0;
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
//    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
//
//    int i = 0;
//    for (const auto& queueFamily : queueFamilies) {
//        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//            indices.graphicsFamily = i;
//        }
//
//        VkBool32 presentSupport = false;
//        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
//
//        if (presentSupport) {
//            indices.presentFamily = i;
//        }
//
//        if (indices.isComplete()) {
//            break;
//        }
//
//        i++;
//    }
//
//    return indices;
//}
//
//
//bool VDevice::isDeviceSuitable(VkPhysicalDevice device) {
//    QueueFamilyIndices indices = findQueueFamilies(device);
//
//    bool extensionsSupported = checkDeviceExtensionSupport(device);
//
//    bool swapChainAdequate = false;
//    if (extensionsSupported) {
//        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
//        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//    }
//
//    VkPhysicalDeviceFeatures supportedFeatures;
//    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
//
//    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
//}
//
//VkSampleCountFlagBits VDevice::getMaxUsableSampleCount() {
//    VkPhysicalDeviceProperties physicalDeviceProperties;
//    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
//
//    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
//    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
//    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
//    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
//    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
//    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
//    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
//
//    return VK_SAMPLE_COUNT_1_BIT;
//}
//
//void VDevice::pickPhysicalDevice() {
//    uint32_t deviceCount = 0;
//    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
//
//    if (deviceCount == 0) {
//        throw std::runtime_error("failed to find GPUs with Vulkan support!");
//    }
//
//    std::vector<VkPhysicalDevice> devices(deviceCount);
//    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
//
//    for (const auto& device : devices) {
//        if (VDevice::isDeviceSuitable(device)) {
//            physicalDevice = device;
//            msaaSamples = getMaxUsableSampleCount();
//            break;
//        }
//    }
//
//    if (physicalDevice == VK_NULL_HANDLE) {
//        throw std::runtime_error("failed to find a suitable GPU!");
//        }
//}
//
//void VDevice::createLogicalDevice() {
//    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
//
//    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
//    std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };
//
//    float queuePriority = 1.0f;
//    for (uint32_t queueFamily : uniqueQueueFamilies) {
//        VkDeviceQueueCreateInfo queueCreateInfo{};
//        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//        queueCreateInfo.queueFamilyIndex = queueFamily;
//        queueCreateInfo.queueCount = 1;
//        queueCreateInfo.pQueuePriorities = &queuePriority;
//        queueCreateInfos.push_back(queueCreateInfo);
//    }
//
//    VkPhysicalDeviceFeatures deviceFeatures{};
//    deviceFeatures.samplerAnisotropy = VK_TRUE;
//
//    VkDeviceCreateInfo createInfo{};
//    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//
//    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
//    createInfo.pQueueCreateInfos = queueCreateInfos.data();
//
//    createInfo.pEnabledFeatures = &deviceFeatures;
//
//    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
//    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
//
//    if (enableValidationLayers) {
//        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
//        createInfo.ppEnabledLayerNames = validationLayers.data();
//    }
//    else {
//        createInfo.enabledLayerCount = 0;
//    }
//
//    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
//        throw std::runtime_error("failed to create logical device!");
//    }
//
//    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
//    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
//    }
//
//bool VDevice::checkDeviceExtensionSupport(VkPhysicalDevice device) {
//    uint32_t extensionCount;
//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
//
//    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
//    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
//
//    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
//
//    for (const auto& extension : availableExtensions) {
//        requiredExtensions.erase(extension.extensionName);
//    }
//
//    return requiredExtensions.empty();
//}
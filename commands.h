#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Commands {
public:
    VkDevice* device;
    VkCommandPool commandPool;
    VkPhysicalDevice* physicalDevice;
    VkSurfaceKHR* surface;
    std::vector<VkCommandBuffer> commandBuffers;
    //std::vector<VkCommandBuffer> computeCommandBuffers;

    Commands();
    Commands(VkDevice* device, VkPhysicalDevice* physicalDevice, VkSurfaceKHR* surface);
    void createCommandPool();
    static VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
    VkCommandBuffer beginSingleTimeCommands();
    static void endSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
    void endSingleTimeCommands(VkQueue graphicsQueue, VkCommandBuffer commandBuffer);
    void createCommandBuffers();
    //void createComputeCommandBuffers();
    void copyBufferToImage(VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};

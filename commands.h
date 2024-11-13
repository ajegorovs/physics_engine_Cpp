#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>

class Commands {
public:
    VkCommandPool commandPool;
    std::vector<VkCommandBuffer> commandLBVHComputeBuffer;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<VkCommandBuffer> computeCommandBuffers;
    
    Commands();
    Commands(VkDevice * pDevice, VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface);
    void createCommandPool();
    static VkCommandBuffer beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool);
    VkCommandBuffer beginSingleTimeCommands();
    static void endSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkCommandBuffer commandBuffer);
    void endSingleTimeCommands(VkQueue graphicsQueue, VkCommandBuffer commandBuffer);
    void createCommandBuffers();
    void createComputeCommandBuffers();
    void createLBVHComputeCommandBuffer();
    void copyBufferToImage(VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
private:
    VkDevice* pDevice;
    VkPhysicalDevice* pPhysicalDevice;
    VkSurfaceKHR* pSurface;

};

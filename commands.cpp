#include "commands.h"
#include "config.h"
#include "device2.h"
#include <iostream>
#include <array>

Commands::Commands() {};

Commands::Commands(VkDevice * pDevice, VkPhysicalDevice * pPhysicalDevice, VkSurfaceKHR * pSurface): 
    pDevice(pDevice), pPhysicalDevice(pPhysicalDevice), pSurface(pSurface)
{
}

void Commands::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = Device2::findQueueFamilies(*pSurface, *pPhysicalDevice);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsAndComputeFamily.value();

    if (vkCreateCommandPool(*pDevice, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics command pool!");
    }
}
VkCommandBuffer Commands::beginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

VkCommandBuffer Commands::beginSingleTimeCommands()
{
    return beginSingleTimeCommands(*pDevice, commandPool);
}

void Commands::endSingleTimeCommands(VkDevice device, VkQueue graphicsQueue, VkCommandPool commandPool, VkCommandBuffer commandBuffer) {
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}
void Commands::endSingleTimeCommands(VkQueue graphicsQueue, VkCommandBuffer commandBuffer) {
    endSingleTimeCommands(*pDevice, graphicsQueue, commandPool, commandBuffer);
}

void Commands::createCommandBuffers() {
    commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(*pDevice, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void Commands::createComputeCommandBuffers()
{
    computeCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)computeCommandBuffers.size();

    if (vkAllocateCommandBuffers(*pDevice, &allocInfo, computeCommandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate compute command buffers!");
    }
}


void Commands::copyBufferToImage(VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };
    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    endSingleTimeCommands(graphicsQueue, commandBuffer);
}

void Commands::createLBVHComputeCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(*pDevice, &allocInfo, &commandLBVHComputeBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate compute command buffers!");
    }
}

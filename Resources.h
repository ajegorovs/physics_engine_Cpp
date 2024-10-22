#pragma once

#ifndef RESOURCES_H
#define RESOURCES_H

#include <memory>
#include <vector>

#include "BaseEnvironment.h"
#include "Device.h"
#include "Image.h"

#include "structs.h"
#include "config.h"



class Resources {
private:
    std::shared_ptr<BaseEnvironment> base; // Shared pointer to the base environment
    std::shared_ptr<Device> dvc;
    std::shared_ptr<Image> img;

    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    VkCommandPool commandPool;

    uint32_t mipLevels;

    VkSampler textureSampler;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    VkDescriptorPool descriptorPool;
    std::vector<VkDescriptorSet> descriptorSets;

    VkBuffer vertexBuffer;
    VkBuffer indexBuffer;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<void*> uniformBuffersMapped;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    uint32_t currentFrame = 0;

    VkDeviceMemory colorImageMemory;
    VkDeviceMemory depthImageMemory;
    VkDeviceMemory textureImageMemory;
    VkDeviceMemory indexBufferMemory;
    VkDeviceMemory vertexBufferMemory;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    Timer timer;

public:
    std::vector<VkFramebuffer> swapChainFramebuffers;

    Resources(std::shared_ptr<BaseEnvironment> env) : base(env) {}

    std::vector<char> readFile(const std::string& filename);

    void cleanup();

    void cleanupSwapChain();

    void connect(std::shared_ptr<Device> env, std::shared_ptr<Image> env2);

    void createDescriptorSetLayout();

    VkShaderModule createShaderModule(const std::vector<char>& code);

    void createGraphicsPipeline();

    void createCommandPool();

    void createColorResources();

    void createDepthResources();

    void createFramebuffers();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    VkCommandBuffer beginSingleTimeCommands();

    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);

    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

    void createTextureImageView();

    void createTextureSampler();

    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createTextureImage();

    void loadModel();

    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    void createVertexBuffer();

    void createIndexBuffer();

    void createUniformBuffers();

    void createDescriptorPool();

    void createDescriptorSets();

    void createCommandBuffers();

    void createSyncObjects();

    void updateUniformBuffer(uint32_t currentImage);

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void recreateSwapChain();

    void drawFrame();

};

#endif
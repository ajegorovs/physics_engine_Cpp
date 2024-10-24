#pragma once
//#include "config.h"
//#include "BaseEnvironment.h"
//#include "Device.h"

#ifndef IMAGE_H
#define IMAGE_H

#include <memory>
#include <vector>
#include <algorithm>

#include "BaseEnvironment.h"
#include "Device.h"


class Image {
private:
    std::shared_ptr<BaseEnvironment> base; // Shared pointer to the base environment
    std::shared_ptr<Device> dvc;
    Timer timer;
public:
    VkSwapchainKHR swapChain;
    std::vector<VkImageView> swapChainImageViews;
    VkRenderPass renderPass;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    VkImage textureImage;
    VkImage colorImage;
    VkImage depthImage;
    VkImageView textureImageView;
    VkImageView colorImageView;
    VkImageView depthImageView;

    Image(std::shared_ptr<BaseEnvironment> env) : base(env) {}

    void connect(std::shared_ptr<Device> env);

    void cleanupSwapChain();
    void cleanup();

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void createSwapChain();


    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

    void createImageViews();

    void createRenderPass();

    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

};
#endif
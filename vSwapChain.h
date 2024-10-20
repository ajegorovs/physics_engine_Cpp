// VulkanSwapChain.hpp
#pragma once
#include <vulkan/vulkan.h>
#include "vDevice.h"

class VSwapChain {
public:
    void create(VDevice& device);
    void cleanup();

private:
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> framebuffers;
};


// VulkanPipeline.hpp
#pragma once
#include <vulkan/vulkan.h>
#include "vDevice.h"

class VulkanPipeline {
public:
    void create(VDevice& device);
    void cleanup();

private:
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;
};

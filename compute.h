#pragma once
#include "misc.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


/// <summary>
/// Comments: 
/// Particle compute pipeline should only have access to re-writable buffer of particle parameters
/// and helper variables like time. 
/// Bindings: 0-> time, 1-> previous frame particle info 2-> current frame particle info
/// Lets outsource MVP calculation to vertex shader.
/// </summary>
class Compute {
public:
    VkPipelineLayout computePipelineLayout;
    VkPipeline computePipeline;
    Compute();
    Compute(VkDevice* pDevice);
    void createComputePipeline_particle(VkDescriptorSetLayout* pDescriptorSetLayout);
    void cleanup();
private:
    VkDevice* pDevice;
    void createComputePipeline(VkPipeline* pComputePipeline, VkPipelineLayout* pPipelineLayout, VkDescriptorSetLayout* pDescriptorSetLayout, std::string shaderDir);
};
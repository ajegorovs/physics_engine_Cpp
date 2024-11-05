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
    VkPipelineLayout computePL_lbvh_particles_update;
    VkPipelineLayout computePL_lbvh_bounding_box_update;
    VkPipelineLayout computePL_lbvh_morton_codes;
    VkPipelineLayout computePL_lbvh_single_radixsort;
    VkPipelineLayout computePL_lbvh_hierarchy;
    VkPipelineLayout computePL_lbvh_bounding_boxes;

    VkPipeline computePipeline;
    VkPipeline computeP_lbvh_particles_update;
    VkPipeline computeP_lbvh_bounding_box_update;
    VkPipeline computeP_lbvh_morton_codes;
    VkPipeline computeP_lbvh_single_radixsort;
    VkPipeline computeP_lbvh_hierarchy;
    VkPipeline computeP_lbvh_bounding_boxes;
    Compute();
    Compute(VkDevice* pDevice);
    void createComputePipeline_particle(
        VkDescriptorSetLayout* pDescriptorSetLayout
    );
    void createComputePipeline_lbvh(
        std::vector<VkDescriptorSetLayout>& descriptorSetLayouts
    );
    void cleanup();
private:
    VkDevice* pDevice;
    void createComputePipeline(VkPipeline* pComputePipeline, VkPipelineLayout* pPipelineLayout, VkDescriptorSetLayout* pDescriptorSetLayout, std::string shaderDir);
    void createComputePipeline(VkPipeline* pComputePipeline, VkPipelineLayout* pPipelineLayout, std::vector<VkDescriptorSetLayout>& pDescriptorSetLayout, uint32_t pushConstSize, std::string shaderDir);
};
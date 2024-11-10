#include "compute.h"
#include "structs.h"
#include "misc.h"
#include <iostream>
#include <cstdint>
#include <stdexcept>

Compute::Compute(){}

Compute::Compute(VkDevice* pDevice) : pDevice(pDevice){}

void Compute::createComputePipeline_particle(VkDescriptorSetLayout* pDescriptorSetLayout)
{
    createComputePipeline(
        &computePipeline,
        &computePipelineLayout,
        pDescriptorSetLayout,
        "shaders/comp.spv");
}


void Compute::createComputePipeline(VkPipeline* pComputePipeline, VkPipelineLayout* pPipelineLayout, VkDescriptorSetLayout* pDescriptorSetLayout, std::string shaderDir)
{
    auto computeShaderCode = Misc::readFile(shaderDir);

    VkShaderModule computeShaderModule = Misc::createShaderModule(pDevice, computeShaderCode);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = pDescriptorSetLayout;

    if (vkCreatePipelineLayout(*pDevice, &pipelineLayoutInfo, nullptr, pPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline layout!");
    }

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = *pPipelineLayout;
    pipelineInfo.stage = computeShaderStageInfo;

    if (vkCreateComputePipelines(*pDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pComputePipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }

    vkDestroyShaderModule(*pDevice, computeShaderModule, nullptr);
}

void Compute::createComputePipeline(VkPipeline* pComputePipeline, VkPipelineLayout* pPipelineLayout, std::vector<VkDescriptorSetLayout>& pDescriptorSetLayout, uint32_t pushConstSize, std::string shaderDir)
{
    auto computeShaderCode = Misc::readFile(shaderDir);

    VkShaderModule computeShaderModule = Misc::createShaderModule(pDevice, computeShaderCode);

    VkPipelineShaderStageCreateInfo computeShaderStageInfo{};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main";

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(pDescriptorSetLayout.size());
    pipelineLayoutInfo.pSetLayouts = pDescriptorSetLayout.data();

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = pushConstSize;

    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(*pDevice, &pipelineLayoutInfo, nullptr, pPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline layout!");
    }

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = *pPipelineLayout;
    pipelineInfo.stage = computeShaderStageInfo;

    if (vkCreateComputePipelines(*pDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pComputePipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute pipeline!");
    }

    vkDestroyShaderModule(*pDevice, computeShaderModule, nullptr);
}


void Compute::createComputePipeline_lbvh(
    std::vector<VkDescriptorSetLayout>& descriptorSetLayouts
)
{

    createComputePipeline(
        &computeP_lbvh_morton_codes,
        &computePL_lbvh_morton_codes,
        descriptorSetLayouts,
        sizeof(PushConstantsMortonCodes),
        "shaders/lbvh_morton_codes.spv");

    createComputePipeline(
        &computeP_lbvh_single_radixsort,
        &computePL_lbvh_single_radixsort,
        descriptorSetLayouts,
        sizeof(PushConstantsRadixSort),
        "shaders/lbvh_single_radixsort.spv");

    createComputePipeline(
        &computeP_lbvh_hierarchy,
        &computePL_lbvh_hierarchy,
        descriptorSetLayouts,
        sizeof(PushConstantsHierarchy),
        "shaders/lbvh_hierarchy.spv");

    createComputePipeline(
        &computeP_lbvh_bounding_boxes,
        &computePL_lbvh_bounding_boxes,
        descriptorSetLayouts,
        sizeof(PushConstantsBoundingBoxes),
        "shaders/lbvh_bounding_boxes.spv");

    createComputePipeline(
        &computeP_lbvh_particles_update,
        &computePL_lbvh_particles_update,
        descriptorSetLayouts,
        sizeof(StructDeltaTimeLBVH),
        "shaders/lbvh_update_particles.spv");

    createComputePipeline(
        &computeP_lbvh_particles_update2,
        &computePL_lbvh_particles_update2,
        descriptorSetLayouts,
        sizeof(StructDeltaTimeLBVH),
        "shaders/lbvh_update_particles_2.spv");

    createComputePipeline(
        &computeP_lbvh_bounding_box_update,
        &computePL_lbvh_bounding_box_update,
        descriptorSetLayouts,
        sizeof(StructDeltaTimeLBVH),
        "shaders/lbvh_update_boxes.spv");
}


void Compute::cleanup()
{
    vkDestroyPipeline(*pDevice, computePipeline, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePipelineLayout, nullptr);

    vkDestroyPipeline(*pDevice, computeP_lbvh_particles_update, nullptr);
    vkDestroyPipeline(*pDevice, computeP_lbvh_bounding_box_update, nullptr);
    vkDestroyPipeline(*pDevice, computeP_lbvh_morton_codes, nullptr);
    vkDestroyPipeline(*pDevice, computeP_lbvh_single_radixsort, nullptr);
    vkDestroyPipeline(*pDevice, computeP_lbvh_hierarchy, nullptr);
    vkDestroyPipeline(*pDevice, computeP_lbvh_bounding_boxes, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePL_lbvh_particles_update, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePL_lbvh_bounding_box_update, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePL_lbvh_morton_codes, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePL_lbvh_single_radixsort, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePL_lbvh_hierarchy, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePL_lbvh_bounding_boxes, nullptr);
}




#include "compute.h"
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

void Compute::cleanup()
{
    vkDestroyPipeline(*pDevice, computePipeline, nullptr);
    vkDestroyPipelineLayout(*pDevice, computePipelineLayout, nullptr);
}



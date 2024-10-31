#include "graphics.h"
#include "misc.h"
#include "structs.h" 
#include <array>
#include <iostream>
#include <cstdint>
#include <fstream>      // For std::ifstream
#include <stdexcept>    // For std::runtime_error

Graphics::Graphics(){}

Graphics::Graphics(VkDevice* pDevice, VkSampleCountFlagBits* pMsaaSamples): pDevice(pDevice), pMsaaSamples(pMsaaSamples){}

void Graphics::createGraphicsPipeline_storageVertices(VkDescriptorSetLayout* pDescriptorSetLayout, VkRenderPass* pRenderPass)
{
    createGraphicsPipeline(
        &graphicsPipeline,
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        &pipelineLayout,
        pDescriptorSetLayout,
        "shaders/vert.spv",
        "shaders/frag.spv",
        Vertex::getBindingDescription(),
        Vertex::getAttributeDescriptions(), 
        pRenderPass);
}

void Graphics::createGraphicsPipeline_storageLines(VkDescriptorSetLayout* pDescriptorSetLayout, VkRenderPass* pRenderPass)
{
    createGraphicsPipeline(
        &graphicsPipeline_lines,
        VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
        &pipelineLayout_lines,
        pDescriptorSetLayout,
        "shaders/vert_Line3D.spv",
        "shaders/frag_Line3D.spv",
        VertexBase::getBindingDescription(),
        VertexBase::getAttributeDescriptions(),
        pRenderPass);
}

void Graphics::createGraphicsPipeline_storageParticles(VkDescriptorSetLayout* pDescriptorSetLayout, VkRenderPass* pRenderPass)
{
    createGraphicsPipeline(
        &graphicsPipeline_particles,
        VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
        &pipelineLayout_particles,
        pDescriptorSetLayout,
        "shaders/vert_Point3D.spv",
        "shaders/frag_Point3D.spv",
        point3D::getBindingDescription(),
        point3D::getAttributeDescriptions(), 
        pRenderPass);
}

void Graphics::createGraphicsPipeline(VkPipeline* pGraphicsPipeline, VkPrimitiveTopology topology, VkPipelineLayout* pPipelineLayout, VkDescriptorSetLayout* pDescriptorSetLayout, std::string vertDir, std::string fragDir, VkVertexInputBindingDescription bindingDescription, std::vector<VkVertexInputAttributeDescription> attributeDescriptions, VkRenderPass* pRenderPass)
{
    auto vertShaderCode = Misc::readFile(vertDir);
    auto fragShaderCode = Misc::readFile(fragDir);

    VkShaderModule vertShaderModule = Misc::createShaderModule(pDevice, vertShaderCode);
    VkShaderModule fragShaderModule = Misc::createShaderModule(pDevice, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = topology;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL; //VK_POLYGON_MODE_LINE;
    rasterizer.lineWidth = 3.0;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = *pMsaaSamples;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
        VK_DYNAMIC_STATE_PRIMITIVE_TOPOLOGY_EXT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = pDescriptorSetLayout;

    if (vkCreatePipelineLayout(*pDevice, &pipelineLayoutInfo, nullptr, pPipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = *pPipelineLayout;
    pipelineInfo.renderPass = *pRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(*pDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, pGraphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    vkDestroyShaderModule(*pDevice, fragShaderModule, nullptr);
    vkDestroyShaderModule(*pDevice, vertShaderModule, nullptr);
}

void Graphics::cleanup()
{
    vkDestroyPipeline(*pDevice, graphicsPipeline, nullptr);
    vkDestroyPipeline(*pDevice, graphicsPipeline_particles, nullptr);
    vkDestroyPipeline(*pDevice, graphicsPipeline_lines, nullptr);

    vkDestroyPipelineLayout(*pDevice, pipelineLayout, nullptr);
    vkDestroyPipelineLayout(*pDevice, pipelineLayout_particles, nullptr);
    vkDestroyPipelineLayout(*pDevice, pipelineLayout_lines, nullptr);
}
#include "descriptors.h"
#include "config.h"	    // MAX_FRAMES_IN_FLIGHT, PARTICLE_COUNT
#include "structs.h"
#include <array>
#include <stdexcept>

// --------------------------------------------------------
// ---- Descriptors allocate resources for shader use. ----
// --------------------------------------------------------
// Description set layout specifies a collection of resources with specific properties.
// binding          - index of a resource;
// descriptorType   - what kind of resource is bound to a buffer
// stageFlags       - which shader stages have access to resources
// (Layout creation is fixed at pipeline creation.)


Descriptors::Descriptors() {}

Descriptors::Descriptors(VkDevice * pDevice) : pDevice(pDevice) {}

void Descriptors::createDescriptorPool()
{
    // (OLD INFO). 3 unique types
    // * x1 Uniform for vertex (model, view, proj)
    // * x1 Uniform for compute (deltaTimes)
    // * x1 Image for frag      (texture sample)
    // * x1 Storage for vertex  (obj transformations)
    // * x2 Storage for compute, for two time steps.
    // each is duplicated twice for MAX_FRAMES_IN_FLIGHT
    std::array<VkDescriptorPoolSize, 3> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * (10);// <<<
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * (10);// <<<
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * (10);// <<<

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 2; // <<<

    if (vkCreateDescriptorPool(*pDevice, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void Descriptors::createDS_single(VkDescriptorSetLayout * pDescriptorSetLayout, std::vector<VkDescriptorSet>& pDescriptionSet, std::vector<VkBuffer>& pBuffer, unsigned long long range, int binding, VkDescriptorType descriptorType)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, *pDescriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool; // one pool for all
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    pDescriptionSet.resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(*pDevice, &allocInfo, pDescriptionSet.data());
    std::cout << result << std::endl;
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = pBuffer[i];
        bufferInfo.offset = 0;
        bufferInfo.range = range;

        VkWriteDescriptorSet descriptorWrites{};

        descriptorWrites.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites.dstSet = pDescriptionSet[i];
        descriptorWrites.dstBinding = binding;
        descriptorWrites.dstArrayElement = 0;
        descriptorWrites.descriptorType = descriptorType;
        descriptorWrites.descriptorCount = 1;
        descriptorWrites.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(*pDevice, 1, &descriptorWrites, 0, nullptr);
    }
}

void Descriptors::createDS_multi_MPV_TS_TRN(std::vector<VkBuffer> uniformBuffers, std::vector<VkBuffer> storageBuffer, VkImageView textureImageView, VkSampler textureSampler)
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout_multi_MPV_TS_TRN);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_multi_MPV_TS_TRN.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(*pDevice, &allocInfo, descriptorSets_multi_MPV_TS_TRN.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(StructMVP);

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = textureImageView;
        imageInfo.sampler = textureSampler;

        VkDescriptorBufferInfo bufferInfo2{};
        bufferInfo2.buffer = storageBuffer[i];
        bufferInfo2.offset = 0;
        bufferInfo2.range = sizeof(StructObjectTransformations);

        std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets_multi_MPV_TS_TRN[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets_multi_MPV_TS_TRN[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = descriptorSets_multi_MPV_TS_TRN[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &bufferInfo2;

        vkUpdateDescriptorSets(*pDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void Descriptors::createDS_uniformMVP(std::vector<VkBuffer>& pUniformBuffer)
{
    createDS_single(
        &descriptorSetLayout_uniformMVP,
        descriptorSets_uniformMVP,
        pUniformBuffer,
        sizeof(StructMVP),
        0,
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
}

void Descriptors::createDS_storageParticles(std::vector<VkBuffer>& pStorageBuffer)
{
    createDS_single(
        &descriptorSetLayout_storageParticles,
        descriptorSets_storageParticles,
        pStorageBuffer,
        sizeof(point3D) * PARTICLE_COUNT,
        0,
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
}

void Descriptors::createDSL_multi_MPV_TS_TRN()
{
    // here we have 3 (1 custom) bindings for graphics rendering.
    // vertex shader can pass data to fragment shader. So we dont have to make UBO visible to frag.
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutBinding sboLayoutBinding{};
    sboLayoutBinding.binding = 2;
    sboLayoutBinding.descriptorCount = 1;
    sboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    std::array<VkDescriptorSetLayoutBinding, 3> bindings = { uboLayoutBinding, samplerLayoutBinding, sboLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo, nullptr, &descriptorSetLayout_multi_MPV_TS_TRN) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Descriptors::createDSL_storageParticles()
{
    // storage type visible to vertex shader
    VkDescriptorSetLayoutBinding sboLayoutBinding{};
    sboLayoutBinding.binding = 0;
    sboLayoutBinding.descriptorCount = 1;
    sboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &sboLayoutBinding;

    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo, nullptr, &descriptorSetLayout_storageParticles) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Descriptors::createDSL_uniformMVP()
{
    // MVP matrix in uniform buffer, visible to vertex shader.
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo, nullptr, &descriptorSetLayout_uniformMVP) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void Descriptors::cleanupDSL()
{
    vkDestroyDescriptorSetLayout(*pDevice, descriptorSetLayout_multi_MPV_TS_TRN, nullptr);
    vkDestroyDescriptorSetLayout(*pDevice, descriptorSetLayout_uniformMVP, nullptr);
    vkDestroyDescriptorSetLayout(*pDevice, descriptorSetLayout_storageParticles, nullptr);
}

void Descriptors::cleanupDPool()
{
    vkDestroyDescriptorPool(*pDevice, descriptorPool, nullptr);
}

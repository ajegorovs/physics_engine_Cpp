#include "descriptors.h"
#include "config.h"	    // MAX_FRAMES_IN_FLIGHT, PARTICLE_COUNT
#include "structs.h"
#include "physics.h"
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
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * (100);// <<<
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * (100);// <<<
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * (100);// <<<

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 5; // <<<

    VkResult result = vkCreateDescriptorPool(*pDevice, &poolInfo, nullptr, &descriptorPool);
    if (result != VK_SUCCESS) {
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

void Descriptors::createDS_physics_compute( // to descriptorSets_1UC_4SC
    std::vector<VkBuffer>& pUniformBuffer,  // deltaTime
    std::vector<VkBuffer>& pStorageBuffer,  // constants
    std::vector<VkBuffer>& pStorageBuffer2, // attractors
    std::vector<VkBuffer>& pStorageBuffer3  // particle old/new
)
{
    // deltaTime, system constants, attractor params, and x2 particle data for current & old frames
    // We bake notion of old and current frames into bindings using "circular buffering/indexing"
    // see ./images/access_previous_buffer_indexing.png for example with 3 frames-in-flight

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout_1UC_4SC);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_1UC_4SC.resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(*pDevice, &allocInfo, descriptorSets_1UC_4SC.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        VkDescriptorBufferInfo bufferInfo_time{};
        bufferInfo_time.buffer = pUniformBuffer[i];
        bufferInfo_time.offset = 0;
        bufferInfo_time.range = sizeof(StructDeltaTime);

        VkDescriptorBufferInfo bufferInfo_constants{};
        bufferInfo_constants.buffer = pStorageBuffer[i];
        bufferInfo_constants.offset = 0;
        bufferInfo_constants.range = sizeof(StructParticleSystemParams);
        
        VkDescriptorBufferInfo bufferInfo_attractor_params{};
        bufferInfo_attractor_params.buffer = pStorageBuffer2[i];
        bufferInfo_attractor_params.offset = 0;
        bufferInfo_attractor_params.range = sizeof(StructAttractor) * NUM_ATTRACTORS;

        // cross-link frames
        VkDescriptorBufferInfo bufferInfo_frame_prev{};
        bufferInfo_frame_prev.buffer = pStorageBuffer3[(i - 1) % MAX_FRAMES_IN_FLIGHT];
        bufferInfo_frame_prev.offset = 0;
        bufferInfo_frame_prev.range = sizeof(point3D) * PARTICLE_COUNT;

        VkDescriptorBufferInfo bufferInfo_frame_current{};
        bufferInfo_frame_current.buffer = pStorageBuffer3[i];
        bufferInfo_frame_current.offset = 0;
        bufferInfo_frame_current.range = sizeof(point3D) * PARTICLE_COUNT;

        std::array<VkWriteDescriptorSet, 5> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets_1UC_4SC[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo_time;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets_1UC_4SC[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pBufferInfo = &bufferInfo_constants;

        descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[2].dstSet = descriptorSets_1UC_4SC[i];
        descriptorWrites[2].dstBinding = 2;
        descriptorWrites[2].dstArrayElement = 0;
        descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[2].descriptorCount = 1;
        descriptorWrites[2].pBufferInfo = &bufferInfo_attractor_params;

        descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[3].dstSet = descriptorSets_1UC_4SC[i];
        descriptorWrites[3].dstBinding = 3;
        descriptorWrites[3].dstArrayElement = 0;
        descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[3].descriptorCount = 1;
        descriptorWrites[3].pBufferInfo = &bufferInfo_frame_prev;

        descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[4].dstSet = descriptorSets_1UC_4SC[i];
        descriptorWrites[4].dstBinding = 4;
        descriptorWrites[4].dstArrayElement = 0;
        descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[4].descriptorCount = 1;
        descriptorWrites[4].pBufferInfo = &bufferInfo_frame_current;

        vkUpdateDescriptorSets(*pDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void Descriptors::createDS_lbvh(
    VkBuffer& bufferElements, 
    VkBuffer& bufferMortonCode, 
    VkBuffer& bufferMortonCodePingPong, 
    VkBuffer& bufferLBVH, 
    VkBuffer& bufferLBVHConstructionInfo)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = descriptorSetLayout_lbvh.size();
    allocInfo.pSetLayouts = descriptorSetLayout_lbvh.data();

    descriptorSets_lbvh.resize(descriptorSetLayout_lbvh.size());
    VkResult result = vkAllocateDescriptorSets(*pDevice, &allocInfo, descriptorSets_lbvh.data());
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    VkDescriptorBufferInfo bufferElements_info{};
    bufferElements_info.buffer = bufferElements;
    bufferElements_info.offset = 0;
    bufferElements_info.range = sizeof(Element) * NUM_ELEMENTS;

    VkDescriptorBufferInfo bufferMortonCode_info{};
    bufferMortonCode_info.buffer = bufferMortonCode;
    bufferMortonCode_info.offset = 0;
    bufferMortonCode_info.range = sizeof(MortonCodeElement) * NUM_ELEMENTS;

    VkDescriptorBufferInfo bufferMortonCodePingPong_info{};
    bufferMortonCodePingPong_info.buffer = bufferMortonCodePingPong;
    bufferMortonCodePingPong_info.offset = 0;
    bufferMortonCodePingPong_info.range = sizeof(MortonCodeElement) * NUM_ELEMENTS;

    VkDescriptorBufferInfo bufferLBVH_info{};
    bufferLBVH_info.buffer = bufferLBVH;
    bufferLBVH_info.offset = 0;
    bufferLBVH_info.range = sizeof(LBVHNode) * NUM_LBVH_ELEMENTS;

    VkDescriptorBufferInfo LBVHConstructionInfo_info{};
    LBVHConstructionInfo_info.buffer = bufferLBVHConstructionInfo;
    LBVHConstructionInfo_info.offset = 0;
    LBVHConstructionInfo_info.range = sizeof(LBVHConstructionInfo) * NUM_LBVH_ELEMENTS;

    // set 0:
    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
    // (0,0) - m_mortonCodeBuffer
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSets_lbvh[0];
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferMortonCode_info;
    // (0,1) - m_elementsBuffer
    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSets_lbvh[0];
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &bufferElements_info;

    // set 1:
    std::array<VkWriteDescriptorSet, 2> descriptorWrites2{};
    // (1,0) - m_mortonCodeBuffer
    descriptorWrites2[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites2[0].dstSet = descriptorSets_lbvh[1];
    descriptorWrites2[0].dstBinding = 0;
    descriptorWrites2[0].dstArrayElement = 0;
    descriptorWrites2[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites2[0].descriptorCount = 1;
    descriptorWrites2[0].pBufferInfo = &bufferMortonCode_info;
    // (1,1) - m_mortonCodePingPongBuffer
    descriptorWrites2[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites2[1].dstSet = descriptorSets_lbvh[1];
    descriptorWrites2[1].dstBinding = 1;
    descriptorWrites2[1].dstArrayElement = 0;
    descriptorWrites2[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites2[1].descriptorCount = 1;
    descriptorWrites2[1].pBufferInfo = &bufferMortonCodePingPong_info;

    // set 2:
    std::array<VkWriteDescriptorSet, 4> descriptorWrites3{};
    // (2,0) - m_mortonCodeBuffer
    descriptorWrites3[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites3[0].dstSet = descriptorSets_lbvh[2];
    descriptorWrites3[0].dstBinding = 0;
    descriptorWrites3[0].dstArrayElement = 0;
    descriptorWrites3[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites3[0].descriptorCount = 1;
    descriptorWrites3[0].pBufferInfo = &bufferMortonCode_info;
    // (2,1) - m_elementsBuffer
    descriptorWrites3[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites3[1].dstSet = descriptorSets_lbvh[2];
    descriptorWrites3[1].dstBinding = 1;
    descriptorWrites3[1].dstArrayElement = 0;
    descriptorWrites3[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites3[1].descriptorCount = 1;
    descriptorWrites3[1].pBufferInfo = &bufferElements_info;
    // (2,2) - m_LBVHBuffer
    descriptorWrites3[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites3[2].dstSet = descriptorSets_lbvh[2];
    descriptorWrites3[2].dstBinding = 2;
    descriptorWrites3[2].dstArrayElement = 0;
    descriptorWrites3[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites3[2].descriptorCount = 1;
    descriptorWrites3[2].pBufferInfo = &bufferLBVH_info;
    // (2,3) - m_LBVHConstructionInfoBuffer
    descriptorWrites3[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites3[3].dstSet = descriptorSets_lbvh[2];
    descriptorWrites3[3].dstBinding = 3;
    descriptorWrites3[3].dstArrayElement = 0;
    descriptorWrites3[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites3[3].descriptorCount = 1;
    descriptorWrites3[3].pBufferInfo = &LBVHConstructionInfo_info;

    //set 3:
    std::array<VkWriteDescriptorSet, 2> descriptorWrites4{};
    // (3,0) - m_LBVHBuffer
    descriptorWrites4[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites4[0].dstSet = descriptorSets_lbvh[3];
    descriptorWrites4[0].dstBinding = 0;
    descriptorWrites4[0].dstArrayElement = 0;
    descriptorWrites4[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites4[0].descriptorCount = 1;
    descriptorWrites4[0].pBufferInfo = &bufferLBVH_info;
    // (3,1) - m_LBVHConstructionInfoBuffer
    descriptorWrites4[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites4[1].dstSet = descriptorSets_lbvh[3];
    descriptorWrites4[1].dstBinding = 1;
    descriptorWrites4[1].dstArrayElement = 0;
    descriptorWrites4[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites4[1].descriptorCount = 1;
    descriptorWrites4[1].pBufferInfo = &LBVHConstructionInfo_info;

    vkUpdateDescriptorSets(*pDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    vkUpdateDescriptorSets(*pDevice, static_cast<uint32_t>(descriptorWrites2.size()), descriptorWrites2.data(), 0, nullptr);
    vkUpdateDescriptorSets(*pDevice, static_cast<uint32_t>(descriptorWrites3.size()), descriptorWrites3.data(), 0, nullptr);
    vkUpdateDescriptorSets(*pDevice, static_cast<uint32_t>(descriptorWrites4.size()), descriptorWrites4.data(), 0, nullptr);
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


void Descriptors::createDSL_1UC_4SC()
{
    // For particles compute pipeline. 
    // bindings: time, particle params old and current

    VkDescriptorSetLayoutBinding ubo{};
    ubo.binding             = 0;
    ubo.descriptorCount     = 1;
    ubo.descriptorType      = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ubo.stageFlags          = VK_SHADER_STAGE_COMPUTE_BIT;
    
    VkDescriptorSetLayoutBinding sbo1{};
    sbo1.binding            = 1;
    sbo1.descriptorCount    = 1;
    sbo1.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo1.stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding sbo2{};
    sbo2.binding            = 2;
    sbo2.descriptorCount    = 1;
    sbo2.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo2.stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding sbo3{};
    sbo3.binding            = 3;
    sbo3.descriptorCount    = 1;
    sbo3.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo3.stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding sbo4{};
    sbo4.binding            = 4;
    sbo4.descriptorCount    = 1;
    sbo4.descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo4.stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;

    std::array<VkDescriptorSetLayoutBinding, 5> bindings = { 
        ubo,
        sbo1, 
        sbo2, 
        sbo3, 
        sbo4 
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
       

    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo, nullptr, &descriptorSetLayout_1UC_4SC) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }
}

void Descriptors::createDSL_lbvh()
{
    descriptorSetLayout_lbvh.resize(4);
    // (set,index): (0,1),(2,1)
    VkDescriptorSetLayoutBinding sbo_elem{};
    sbo_elem.binding = 1;
    sbo_elem.descriptorCount = 1;
    sbo_elem.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo_elem.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    // (0,0),(1,0),(2,0)
    VkDescriptorSetLayoutBinding sbo_mortonCode{};
    sbo_mortonCode.binding = 0;
    sbo_mortonCode.descriptorCount = 1;
    sbo_mortonCode.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo_mortonCode.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    // (1,1)
    VkDescriptorSetLayoutBinding sbo_mortonCodePingPong{};
    sbo_mortonCodePingPong.binding = 1;
    sbo_mortonCodePingPong.descriptorCount = 1;
    sbo_mortonCodePingPong.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo_mortonCodePingPong.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    // (2,2),(3,0)
    VkDescriptorSetLayoutBinding sbo_LBVH{};
    sbo_LBVH.binding = 0;
    sbo_LBVH.descriptorCount = 1;
    sbo_LBVH.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo_LBVH.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    
    VkDescriptorSetLayoutBinding sbo_LBVH2{};
    sbo_LBVH2.binding = 2;
    sbo_LBVH2.descriptorCount = 1;
    sbo_LBVH2.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo_LBVH2.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    // (2,3),(3,1)
    VkDescriptorSetLayoutBinding sbo_LBVHConstructionInfo{};
    sbo_LBVHConstructionInfo.binding = 1;
    sbo_LBVHConstructionInfo.descriptorCount = 1;
    sbo_LBVHConstructionInfo.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo_LBVHConstructionInfo.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding sbo_LBVHConstructionInfo2{};
    sbo_LBVHConstructionInfo2.binding = 3;
    sbo_LBVHConstructionInfo2.descriptorCount = 1;
    sbo_LBVHConstructionInfo2.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    sbo_LBVHConstructionInfo2.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    // Set 0:
    std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
        sbo_elem,                 // (0,1)
        sbo_mortonCode            // (0,0)
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();


    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo, nullptr, &descriptorSetLayout_lbvh[0]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }
    // Set 1:
    std::array<VkDescriptorSetLayoutBinding, 2> bindings2 = {
        sbo_mortonCode,             // (1,0)
        sbo_mortonCodePingPong      // (1,1)
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo2{};
    layoutInfo2.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo2.bindingCount = static_cast<uint32_t>(bindings2.size());
    layoutInfo2.pBindings = bindings2.data();


    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo2, nullptr, &descriptorSetLayout_lbvh[1]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }
    // set 2:
    std::array<VkDescriptorSetLayoutBinding, 4> bindings3 = {
        sbo_elem,                    // (2,1)
        sbo_mortonCode,              // (2,0)
        sbo_LBVH2,                   // (2,2)
        sbo_LBVHConstructionInfo2    // (2,3)
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo3{};
    layoutInfo3.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo3.bindingCount = static_cast<uint32_t>(bindings3.size());
    layoutInfo3.pBindings = bindings3.data();


    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo3, nullptr, &descriptorSetLayout_lbvh[2]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }

    // set 3:
    std::array<VkDescriptorSetLayoutBinding, 2> bindings4 = {
        sbo_LBVH,                  // (3,0)
        sbo_LBVHConstructionInfo,  // (3,1)
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo4{};
    layoutInfo4.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo4.bindingCount = static_cast<uint32_t>(bindings4.size());
    layoutInfo4.pBindings = bindings4.data();


    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo4, nullptr, &descriptorSetLayout_lbvh[3]) != VK_SUCCESS) {
        throw std::runtime_error("failed to create compute descriptor set layout!");
    }

}

void Descriptors::cleanupDSL()
{
    vkDestroyDescriptorSetLayout(*pDevice, descriptorSetLayout_multi_MPV_TS_TRN, nullptr);
    vkDestroyDescriptorSetLayout(*pDevice, descriptorSetLayout_uniformMVP, nullptr);
    vkDestroyDescriptorSetLayout(*pDevice, descriptorSetLayout_1UC_4SC, nullptr);

    for (VkDescriptorSetLayout DSL : descriptorSetLayout_lbvh) {
        vkDestroyDescriptorSetLayout(*pDevice, DSL, nullptr);
    }
    

}

void Descriptors::cleanupDPool()
{
    vkDestroyDescriptorPool(*pDevice, descriptorPool, nullptr);
}

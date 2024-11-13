#include "descriptors.h"
#include "config.h"	    // MAX_FRAMES_IN_FLIGHT, PARTICLE_COUNT
#include "structs.h"
#include "physics.h"
#include "lbvh.h"
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
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) * 7; // <<<

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


void Descriptors::updateDescriptorSets(
    VkDescriptorSet descriptorSet,
    std::vector<VkDescriptorBufferInfo> descriptorBufferInfos,
    std::vector<VkDescriptorType> descriptorTypes
    )
{
    uint32_t num_bindings = static_cast<uint32_t>(descriptorBufferInfos.size());
    std::vector<VkWriteDescriptorSet> descriptorWrites(num_bindings);

    for (uint32_t i = 0; i < num_bindings; i++)
    {
        descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet = descriptorSet;
        descriptorWrites[i].dstBinding = i;
        descriptorWrites[i].dstArrayElement = 0;
        descriptorWrites[i].descriptorType = descriptorTypes[i];
        descriptorWrites[i].descriptorCount = 1;
        descriptorWrites[i].pBufferInfo = &descriptorBufferInfos[i];
        std::cout << "\tBinding: "<< i << ", Type : "<< descriptorTypes[i] << std::endl;
    }
    vkUpdateDescriptorSets(*pDevice, num_bindings, descriptorWrites.data(), 0, nullptr);
}

void Descriptors::createDS_lbvh(
    std::vector<VkBuffer>& bufferMortonCode,
    std::vector<VkBuffer>& bufferMortonCodePingPong,
    std::vector<VkBuffer>& bufferLBVH,
    std::vector<VkBuffer>& bufferLBVHConstructionInfo,
    std::vector<VkBuffer>& bufferLBVHParticles,
    std::vector<VkBuffer>& bufferGlobalBB
)
{
    uint32_t num_sets = static_cast<uint32_t>(descriptorSetLayout_lbvh.size());

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = num_sets;
    allocInfo.pSetLayouts = descriptorSetLayout_lbvh.data();

    descriptorSets_lbvh.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        std::cout << "Preparing descriptor set for frame-in-flight #" << i << " :" << std::endl;
        descriptorSets_lbvh[i].resize(num_sets);

        VkResult result = vkAllocateDescriptorSets(*pDevice, &allocInfo, descriptorSets_lbvh[i].data());
        if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        VkDescriptorBufferInfo bufferMortonCode_info{ bufferMortonCode[i] , 0, sizeof(MortonCodeElement) * NUM_ELEMENTS};

        VkDescriptorBufferInfo bufferMortonCodePingPong_info{ bufferMortonCodePingPong[i] , 0, sizeof(MortonCodeElement) * NUM_ELEMENTS };

        VkDescriptorBufferInfo bufferLBVH_info{ bufferLBVH[i] , 0, sizeof(LBVHNode) * NUM_LBVH_ELEMENTS };

        VkDescriptorBufferInfo LBVHConstructionInfo_info{ bufferLBVHConstructionInfo[i] , 0, sizeof(LBVHConstructionInfo) * NUM_LBVH_ELEMENTS };

        VkDescriptorBufferInfo bufferParticleInfo_old{ bufferLBVHParticles[(i - 1) % MAX_FRAMES_IN_FLIGHT] , 0, sizeof(point3D) * NUM_ELEMENTS };

        VkDescriptorBufferInfo bufferParticleInfo{ bufferLBVHParticles[i] , 0, sizeof(point3D) * NUM_ELEMENTS };

        VkDescriptorBufferInfo bufferGlobalBBInfo{ bufferGlobalBB[i] , 0, sizeof(GlobalBoundingBox) };

        // Bindings in order of entries.
        std::vector<std::vector<VkDescriptorBufferInfo>> descriptorBufferInfos = { // change DSL binding counts!
            {bufferMortonCode_info  , bufferParticleInfo, bufferGlobalBBInfo},                                 //   0-morton code MC
            {bufferMortonCode_info  , bufferMortonCodePingPong_info},                                          //   1-radix sort of MC
            {bufferMortonCode_info  , bufferParticleInfo       , bufferLBVH_info, LBVHConstructionInfo_info},  //   2-hierarchy
            {bufferLBVH_info        , LBVHConstructionInfo_info},                                              //   3-update tree
            {bufferParticleInfo     , bufferParticleInfo_old, bufferLBVH_info},                                //   4-update forces
            {bufferParticleInfo     , bufferGlobalBBInfo}                                                      //   5-update BBs
        }; // { 3,2,4,2,3,2 }; 

        // all descriptor types will be storages
        size_t size;
        std::vector<VkDescriptorType> descriptorTypes;
        for (uint32_t k = 0; k < num_sets; k++)
        {
            std::cout << "Set: " << k << std::endl;
            size = descriptorBufferInfos[k].size();
            descriptorTypes = std::vector<VkDescriptorType>(size, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);

            updateDescriptorSets(
                descriptorSets_lbvh[i][k],
                descriptorBufferInfos[k],
                descriptorTypes
            );
        }

    }
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

void Descriptors::createDescriptorSetLayout(
    std::vector<VkDescriptorType> descriptorTypes,
    std::vector<VkShaderStageFlagBits> shaderStageFlagBits,
    VkDescriptorSetLayout* pDescriptorSetLayout
)
{
    /// Fills descriptor type and stage flags for a set. Ofc binding as in order.
    uint32_t num_bindings = static_cast<uint32_t>(descriptorTypes.size());
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings;
    descriptorSetLayoutBindings.reserve(num_bindings);

    for (uint32_t i = 0; i < num_bindings; i++)
    {
        VkDescriptorSetLayoutBinding binding;
        binding.binding = i;
        binding.descriptorCount = 1;
        binding.descriptorType = descriptorTypes[i];
        binding.stageFlags = shaderStageFlagBits[i];

        descriptorSetLayoutBindings.push_back(binding);

        std::cout << "\tBinding: " << i << ", Type: " << descriptorTypes[i] << std::endl;
    }
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = num_bindings;
    layoutInfo.pBindings = descriptorSetLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(*pDevice, &layoutInfo, nullptr, pDescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create a descriptor set layout!");
    }
}
void Descriptors::createDSL_lbvh()
{
    std::cout << "Preparing descriptor set layout:" << std::endl;
    // All sets and bindings are of type storage buffer and are used by compute stage;
    std::vector<uint32_t> bindings_per_set = { 3,2,4,2,3,2 }; 

    uint32_t num_sets = static_cast<uint32_t>(bindings_per_set.size());

    descriptorSetLayout_lbvh.resize(num_sets);

    std::vector<VkDescriptorType> descriptorTypes;
    std::vector<VkShaderStageFlagBits> shaderStageFlagBits;

    for (uint32_t i = 0; i < num_sets; i++)
    {
        std::cout << "Set: " << i << std::endl;
        descriptorTypes = std::vector<VkDescriptorType>(            bindings_per_set[i], VK_DESCRIPTOR_TYPE_STORAGE_BUFFER);
        shaderStageFlagBits = std::vector<VkShaderStageFlagBits>(   bindings_per_set[i], VK_SHADER_STAGE_COMPUTE_BIT);

        createDescriptorSetLayout(
            descriptorTypes,
            shaderStageFlagBits,
            &descriptorSetLayout_lbvh[i]
        );
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

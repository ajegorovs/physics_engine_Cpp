#include "buffers.h"
#include "device2.h"
#include "commands.h"
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <random>       // default_random_engine, uniform_real_distribution
#include <cmath> 
#include <glm/glm.hpp>

Buffers::Buffers(){}

Buffers::Buffers(VkDevice * pDevice, VkPhysicalDevice* physicalDevice, VkCommandPool* commandPool, VkQueue* graphicsQueue) :
    pDevice(pDevice), physicalDevice(physicalDevice), commandPool(commandPool), graphicsQueue(graphicsQueue){}


void Buffers::processScene(const std::vector<std::unique_ptr<geometric_shape>>& pScene) {
    float id = 0;
    for (const auto& shape : pScene) {

        for (uint32_t i = 0; i < shape->colors.size(); i++) {
            glm::vec4 coord = shape->vertices[i];
            glm::vec3 color = shape->colors[i];
            //std::cout << glm::to_string(coord) << std::endl;
            Vertex vertex{ {coord.x,coord.y,coord.z}, {color.x,color.y,color.z}, {0.0f, 0.0f}, 0.0f,  id };
            vertices.push_back(vertex);

        }

        uint32_t maxValue = 0;
        uint32_t offset = 0;
        if (!indices.empty()) {
            auto maxIt = std::max_element(indices.begin(), indices.end());
            maxValue = *maxIt;
            offset = 1;
        }

        for (auto& index : shape->indices) {
            indices.push_back(index + offset + maxValue);
        }
        id++;

    }
}

// non-local: createTextureImage, 
void Buffers::createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{   // information for buffer -  size, what is its use. 
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // gives a handle for, still abstract buffer object. we have to fill it.
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }
    // additional checks for memory type
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
    // allocate space for buffer. idk about memory type.
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = Device2::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate buffer memory!");
    }
    // make allocated memory usable.
    vkBindBufferMemory(device, buffer, bufferMemory, 0);
}
;
// local createVertexBuffer, createIndexBuffer, createUniformBuffers, static buffer.
void Buffers::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    createBuffer(*pDevice, *physicalDevice, size, usage, properties, buffer, bufferMemory);
}

// local createVertexBuffer, createIndexBuffer,  createUniformBuffers, static buffer
void Buffers::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = Commands::beginSingleTimeCommands(*pDevice, *commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    Commands::endSingleTimeCommands(*pDevice, *graphicsQueue,*commandPool, commandBuffer);
}

void Buffers::createIndexBuffer() {
    // CPU (host) does not have access to GPU (device, local) memory. Its "invisible". 
    // staging buffer is "host-visible" so it can act as inermediate stage.
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    // GOAL - transfer data from CPU to GPU memory thats accessable by GPU
    // PROPERTY - target is a memory block on GPU can be seen by CPU (HOST_VISIBLE)
    // USE -  this buffer will be used to send data (TRANSFER_SRC_BIT)
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    // copy data form cpu to GPU
    void* data;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);
    // GOAL - move GPU data to a "better" memory block (also GPU).
    // PROPERTY: target is memory on GPU (DEVICE_LOCAL_BIT)
    // USE: it will recieve data (TRANSFER_DST_BIT) and contained indices (INDEX_BUFFER_BIT)
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
    // copy data on GPU
    copyBuffer( stagingBuffer, indexBuffer, bufferSize);
    // remove old host-visible data copy and references.
    vkDestroyBuffer(*pDevice, stagingBuffer, nullptr);
    vkFreeMemory(*pDevice, stagingBufferMemory, nullptr);
    // Comment: Having staging buffer being SRC and final buffer being DST sounds right. But tbh it does not make 100% sense, as both targets are on GPU.
}

void Buffers::createVertexBuffer() {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

    copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

    vkDestroyBuffer(*pDevice, stagingBuffer, nullptr);
    vkFreeMemory(*pDevice, stagingBufferMemory, nullptr);
}

void Buffers::createBuffer_uniformMVP() {
    // shader data should be on GPU. it at least can be host-visible
    // but better performance would be "deeper" on GPU.
    VkDeviceSize bufferSize = sizeof(StructMVP);
    
    buffer_uniformMVP.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory_uniformMVP.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMapped_uniformMVP.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            bufferSize, 
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            buffer_uniformMVP[i], 
            bufferMemory_uniformMVP[i]
        );

        vkMapMemory(*pDevice, bufferMemory_uniformMVP[i], 0, bufferSize, 0, &bufferMapped_uniformMVP[i]);
        // unmap memory ?
    }
}

void Buffers::createBuffer_storageTransformations() {
    // Custom storage buffer. but same logic of host-visible memory.
    VkDeviceSize bufferSize = sizeof(StructObjectTransformations);

    buffer_storageTransformations.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory_storageTransformations.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMapped_storageTransformtions.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            buffer_storageTransformations[i], bufferMemory_storageTransformations[i]);

        vkMapMemory(*pDevice, bufferMemory_storageTransformations[i], 0, bufferSize, 0, &bufferMapped_storageTransformtions[i]);
    }
}

void Buffers::createBuffer_storageParticles() {
    // Custom storage buffer. but same logic of host-visible memory.
    VkDeviceSize bufferSize = sizeof(point3D) * PARTICLE_COUNT;

    buffer_storageParticles.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory_storageParticles.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMapped_storageParticles.resize(MAX_FRAMES_IN_FLIGHT);

    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(-1.0f, 1.0f);

    // init
    float id = 0;
    std::vector<point3D> particles(PARTICLE_COUNT);
    for (auto& particle : particles) {
        particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 0.5f * 0.5f *(rndDist(rndEngine) + 1.0f));
        particle.position = glm::vec3(rndDist(rndEngine), rndDist(rndEngine), 0.2f + 0.2f*rndDist(rndEngine));
        particle.velocity = glm::ballRand(0.3f);
        particle.acceleration = glm::vec3(0.0f, 0.0f, 0.0f);
        particle.mass = glm::float32(1.0);
        particle.damping = glm::float32(0.0);

        id += 0.1;
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
            buffer_storageParticles[i], bufferMemory_storageParticles[i]);

        // get a pointer bufferMapped_ to a memory block of type bufferMemory_ and of size bufferSize
        vkMapMemory(*pDevice, bufferMemory_storageParticles[i], 0, bufferSize, 0, &bufferMapped_storageParticles[i]);
        // copy particle data of size bufferSize to storage buffer bufferMapped_.
        memcpy(bufferMapped_storageParticles[i], particles.data(), (size_t)bufferSize);
        vkUnmapMemory(*pDevice, bufferMemory_storageParticles[i]);
    }
}
//void Buffers::createShaderStorageBuffers() {
//    // We will store data "deep" in GPU on device-local memory. No access for CPU (?).
//    // we again use staging buffer to transfer data to GPU.
//    VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;
//
//    shaderStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
//    shaderStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
//
//
//    // Initialize particles
//    std::default_random_engine rndEngine((unsigned)time(nullptr));
//    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
//    // generate particele data in RAM
//    std::vector<Particle> particles(PARTICLE_COUNT);
//    for (auto& particle : particles) {
//        float r = 0.25f * sqrt(rndDist(rndEngine));
//        float theta = rndDist(rndEngine) * 2 * 3.14159265358979323846;
//        float x = r * cos(theta) * HEIGHT / WIDTH;
//        float y = r * sin(theta);
//        particle.position = glm::vec2(x, y);
//        particle.velocity = glm::normalize(glm::vec2(x, y)) * 0.00025f;
//        particle.color = glm::vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
//    }
//    // we again use staging buffer to transfer data to GPU. read Buffers::createIndexBuffer().
//    VkBuffer stagingBuffer;
//    VkDeviceMemory stagingBufferMemory;
//    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//    // Copy particle data to GPU
//    void* data;
//    vkMapMemory(*device, stagingBufferMemory, 0, bufferSize, 0, &data);
//    memcpy(data, particles.data(), (size_t)bufferSize);
//    vkUnmapMemory(*device, stagingBufferMemory);
//    // deep on gpu (DEVICE_LOCAL_BIT), gets written in, used as a storage buffer by vertex shader.
//    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//        createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
//            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, shaderStorageBuffers[i], shaderStorageBuffersMemory[i]);
//        copyBuffer(stagingBuffer, shaderStorageBuffers[i], bufferSize);
//    }
//
//    vkDestroyBuffer(*device, stagingBuffer, nullptr);
//    vkFreeMemory(*device, stagingBufferMemory, nullptr);
//}

void Buffers::clearBuffers1(){
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(*pDevice, buffer_uniformMVP[i], nullptr);
        vkDestroyBuffer(*pDevice, buffer_storageTransformations[i], nullptr);
        vkDestroyBuffer(*pDevice, buffer_storageParticles[i], nullptr);

        vkFreeMemory(   *pDevice, bufferMemory_uniformMVP[i], nullptr);
        vkFreeMemory(   *pDevice, bufferMemory_storageTransformations[i], nullptr);
        vkFreeMemory(   *pDevice, bufferMemory_storageParticles[i], nullptr);
    }
}

void Buffers::clearBuffers2(){
    vkDestroyBuffer(*pDevice, indexBuffer, nullptr);
    vkDestroyBuffer(*pDevice, vertexBuffer, nullptr);
    vkFreeMemory(   *pDevice, indexBufferMemory, nullptr);
    vkFreeMemory(   *pDevice, vertexBufferMemory, nullptr);
}


#include "buffers.h"
#include "device2.h"
#include "commands.h"
#include "physics.h"
#include "lbvh.h"
#include <iostream>
#include <cstdint>
#include <algorithm>
#include <random>       // default_random_engine, uniform_real_distribution
#include <cmath> 


Buffers::Buffers(){}

Buffers::Buffers(VkDevice * pDevice, VkPhysicalDevice* physicalDevice, VkCommandPool* commandPool, VkQueue* graphicsQueue) :
    pDevice(pDevice), physicalDevice(physicalDevice), commandPool(commandPool), graphicsQueue(graphicsQueue){}


void Buffers::processScene(const std::vector<std::unique_ptr<geometric_shape>>& pScene) {
    // vertices and indices are temporatry pre-buffer storages/accumulators
    float id = 0;
    for (const auto& shape : pScene) {

        for (uint32_t i = 0; i < shape->colors.size(); i++) {
            glm::vec4 coord = shape->vertices[i];
            glm::vec3 color = shape->colors[i];
            //std::cout << glm::to_string(coord) << std::endl;
            Vertex vertex{ glm::vec3(coord.x,coord.y,coord.z), glm::vec3(color.x,color.y,color.z), glm::vec2(0.0f, 0.0f), 0.0f, id };
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

//void Buffers::processGrid()
//{
//    glm::vec3 P1(-3.0f, -3.0f, 0.0f);
//    glm::vec3 P2( 3.0f,  3.0f, 0.0f);
//    glm::vec3 dims = (P2 - P1);
//
//    int num_h_lines = 8;
//    int num_v_lines = 8;
//
//    glm::vec2 dr = glm::vec2(dims[0] / num_v_lines, dims[1] / num_h_lines);
//
//    glm::vec3 clr(0.0f, 1.0f, 0.0f);
//
//    for (size_t i = 0; i <= num_h_lines; i++) { // + 1 line <=
//
//        VertexBase p1{ glm::vec3(0      , i * dr.y, 0) + P1, clr};
//        VertexBase p2{ glm::vec3(dims.x , i * dr.y, 0) + P1, clr};
//
//        line_vertices.push_back(p1);
//        line_vertices.push_back(p2);
//
//    }
//
//    for (size_t i = 0; i <= num_v_lines; i++) {
//
//        VertexBase p1{ glm::vec3(i * dr.x , 0     ,  0) + P1, clr };
//        VertexBase p2{ glm::vec3(i * dr.x , dims.y,  0) + P1, clr };
//
//        line_vertices.push_back(p1);
//        line_vertices.push_back(p2);
//
//    }
//
//
//}

//void Buffers::processMortonLines(std::vector<std::array<float, 3>> points, glm::vec3 color, glm::vec3 offset)
//{
//    
//    int N = points.size();
//    for (size_t i = 0; i < N - 1; i++)
//    {
//        std::array<float, 3> c = points[i];
//        std::array<float, 3> g = points[i+1];
//        VertexBase p1{ 3.0f * glm::vec3(c[0], c[1], c[2])+offset, color };
//        VertexBase p2{ 3.0f * glm::vec3(g[0], g[1], g[2])+offset, color };
//
//        line_vertices.push_back(p1);
//        line_vertices.push_back(p2);
//    }
//
//}

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

void Buffers::createBufferDeviceLocalData(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    std::vector<VkBuffer>& buffer,
    std::vector<VkDeviceMemory>& bufferMemory,
    const void* ptr
) 
{
    VkDeviceSize bufferSize = size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, ptr, (size_t)bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);

    buffer.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory.resize(MAX_FRAMES_IN_FLIGHT);

    // Copy initial particle data to all storage buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            bufferSize,
            usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            buffer[i],
            bufferMemory[i]);

        copyBuffer(stagingBuffer, buffer[i], bufferSize);
    }
    vkDestroyBuffer(*pDevice, stagingBuffer, nullptr);
    vkFreeMemory(*pDevice, stagingBufferMemory, nullptr);
}

void Buffers::createBufferDeviceLocalData(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    std::vector<VkBuffer>& buffer,
    std::vector<VkDeviceMemory>& bufferMemory
)
{
    VkDeviceSize bufferSize = size;

    buffer.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory.resize(MAX_FRAMES_IN_FLIGHT);

    // Copy initial particle data to all storage buffers
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            bufferSize,
            usage,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            buffer[i],
            bufferMemory[i]);
    }
}
void Buffers::createBufferDeviceLocalData(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory,
    const void* ptr
)
{
    VkDeviceSize bufferSize = size;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, ptr, (size_t)bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);


    createBuffer(
        bufferSize,
        usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        buffer,
        bufferMemory);

    copyBuffer(stagingBuffer, buffer, bufferSize);
    
    vkDestroyBuffer(*pDevice, stagingBuffer, nullptr);
    vkFreeMemory(*pDevice, stagingBufferMemory, nullptr);
}

void Buffers::createBufferDeviceLocalData(
    VkDeviceSize size,
    VkBufferUsageFlags usage,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory
)
{
    VkDeviceSize bufferSize = size;


    // Copy initial particle data to all storage buffers

    createBuffer(
        bufferSize,
        usage,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        buffer,
        bufferMemory);

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

void Buffers::createBuffer_line()
{                                          
    createBuffer(lineVertSize, 
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer_lines, bufferMemory_lines);

    vkMapMemory(*pDevice, bufferMemory_lines, 0, lineVertSize, 0, &bufferMapped_lines);
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


void Buffers::createBuffer_physics_particles_compute()
{
    std::vector<point3D> particles(PARTICLE_COUNT);

    std::vector<float> particle_ids = Particles::getParticleGroupsIDs();

    for (uint32_t i = 0; i < PARTICLE_COUNT; i++)
    {
        particles[i].group_id = particle_ids[i];

    }

    createBufferDeviceLocalData(
        sizeof(point3D) * PARTICLE_COUNT,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        buffer_physics_particles,
        bufferMemory_physics_particles,
        static_cast<const void*>(particles.data())
    );
    //particle_ids.clear();
    //particle_ids.shrink_to_fit();
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
    }
}

void Buffers::createBuffer_lbvh_points(std::vector<glm::vec3> points, glm::vec3 color)
{
    std::vector<point3D> particles;
    particles.reserve(NUM_ELEMENTS);

    for (glm::vec3 p : points)
    {
        glm::vec3 d = glm::vec3(0.5f,0.5f,0.5f) - p;
        glm::float32 dist = glm::length(d);
        glm::vec3 vel_dir = glm::normalize(glm::cross(d, glm::vec3(0.0f,0.0f,1.0f)));
        // num particles at center (volume of 4/3 pi dist^3) is dist^3/R^3 * N = dist^3/(1/2)^3*N = 2^3*dist^3*N
        glm::vec3 orbital_vel = vel_dir * glm::sqrt(NUM_ELEMENTS * 8.0f * dist*dist*dist* GRAV_CONST / (dist + 0.000001f));

        //glm::vec3 orbital_vel = glm::normalize(d);// (0.0f, 0.0f, 0.0f);
        //glm::vec3 orbital_vel(0.007f, 0.008f, 0.0009f);
        //glm::vec3 accel(0.0045f, 0.0046f, 0.0047f);
        glm::vec3 accel(0.0f);
        point3D v{glm::vec4(color,1.0f), p, 0.005f*orbital_vel, accel , 1.0f, 1.0f, 0.0f};
        particles.push_back(v);
    }

    createBufferDeviceLocalData(
        sizeof(point3D) * NUM_ELEMENTS,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT| VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        buffer_lbvh_particles,
        bufferMemory_lbvh_particles,
        static_cast<const void*>(particles.data())
    );
}

void Buffers::createBuffer_lbvh_points_host_vis()
{
    VkDeviceSize size = sizeof(point3D) * NUM_ELEMENTS;
    createBuffer(
        size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer_lbvh_particles_host_vis,
        bufferMemory_lbvh_particles_host_vis);

    vkMapMemory(*pDevice, bufferMemory_lbvh_particles_host_vis, 0,
        size, 0, &bufferMapped_lbvh_particles_host_vis);
}


void Buffers::createBuffer_lbvh_elementsBuffer(std::vector<glm::vec3> points)
{
    std::vector<Element> particles;
    particles.reserve(NUM_ELEMENTS);
    
    for (uint32_t i = 0; i < NUM_ELEMENTS; i++)
    {
        glm::vec3 p = points[i];
        Element e;
        e.primitiveIdx = i;
        e.aabbMinX = p[0] - P_R; // tested in RenderDoc with 0.1f, 0.2f,...
        e.aabbMinY = p[1] - P_R;
        e.aabbMinZ = p[2] - P_R;
        e.aabbMaxX = p[0] + P_R;
        e.aabbMaxY = p[1] + P_R;
        e.aabbMaxZ = p[2] + P_R;
        e.mass = 1.0f;
        particles.push_back(e);
    }

    createBufferDeviceLocalData(
        sizeof(Element) * NUM_ELEMENTS,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        buffer_lbvh_elements,
        bufferMemory_lbvh_elements,
        static_cast<const void*>(particles.data())
    );
    //particle_ids.clear();
    //particle_ids.shrink_to_fit();
}

void Buffers::createBuffer_lbvh_mortonCode()
{
    createBufferDeviceLocalData(
        sizeof(MortonCodeElement) * NUM_ELEMENTS,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        buffer_lbvh_mortonCode,
        bufferMemory_lbvh_mortonCode
    );

}

void Buffers::createBuffer_lbvh_mortonCodePingPong()
{
    createBufferDeviceLocalData(
        sizeof(MortonCodeElement) * NUM_ELEMENTS,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        buffer_lbvh_mortonCodePingPong,
        bufferMemory_lbvh_mortonCodePingPong
    );
}

void Buffers::createBuffer_lbvh_LBVH()
{
    createBufferDeviceLocalData(
        sizeof(LBVHNode) * NUM_LBVH_ELEMENTS,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT| VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        buffer_lbvh_LBVH,
        bufferMemory_lbvh_LBVH
    );
}

void Buffers::createBuffer_lbvh_LBVHConstructionInfo()
{
    createBufferDeviceLocalData(
        sizeof(LBVHConstructionInfo) * NUM_LBVH_ELEMENTS,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        buffer_lbvh_LBVHConstructionInfo,
        bufferMemory_lbvh_LBVHConstructionInfo
    );
}

void Buffers::createBuffer_lbvh_LBVH_hist_vis()
{
    VkDeviceSize size = sizeof(LBVHNode) * NUM_LBVH_ELEMENTS;
    createBuffer(
        size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT| VK_BUFFER_USAGE_TRANSFER_DST_BIT| VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        buffer_lbvh_LBVH_host_vis,
        bufferMemory_lbvh_LBVH_host_vis);

    vkMapMemory(*pDevice, bufferMemory_lbvh_LBVH_host_vis, 0, 
        size, 0, &bufferMapped_lbvh_LBVH_hist_vis);
}

void Buffers::createBuffer_uniformDeltaTime()
{
    VkDeviceSize bufferSize = sizeof(StructDeltaTime);

    buffer_uniformDeltaTime.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory_uniformDeltaTime.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMapped_uniformDeltaTime.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            buffer_uniformDeltaTime[i],
            bufferMemory_uniformDeltaTime[i]
        );

        vkMapMemory(*pDevice, bufferMemory_uniformDeltaTime[i], 0, bufferSize, 0, &bufferMapped_uniformDeltaTime[i]);
    }
}

void Buffers::createBuffer_physics_particles_constants()
{
    // maybe pre-define attractor params in physics.h?
    StructParticleSystemParams particleParams;
    particleParams.num_attractors = NUM_ATTRACTORS;
    particleParams.grav_const = GRAV_CONST;
    particleParams.particle_count = PARTICLE_COUNT;
    particleParams.blob_r_min = BLOB_R_MIN;
    particleParams.blob_r_max = BLOB_R_MAX;

    VkDeviceSize bufferSize = sizeof(particleParams);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    void* data;
    vkMapMemory(*pDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, &particleParams, (size_t)bufferSize);
    vkUnmapMemory(*pDevice, stagingBufferMemory);

    buffer_physics_constants.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory_physics_constants.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            buffer_physics_constants[i],
            bufferMemory_physics_constants[i]);
        copyBuffer(stagingBuffer, buffer_physics_constants[i], bufferSize);
    }

    vkDestroyBuffer(*pDevice, stagingBuffer, nullptr);
    vkFreeMemory(*pDevice, stagingBufferMemory, nullptr);
    //delete &particleParams;
}

void Buffers::createBuffer_physics_attractors(std::vector<float> masses, std::vector<float> radiuses, std::vector<float> densities, std::vector<glm::vec3> positions)
{
    VkDeviceSize bufferSize = sizeof(StructAttractor) * NUM_ATTRACTORS;
    std::vector< StructAttractor> attractors;
    attractors.reserve(NUM_ATTRACTORS);

    for (size_t i = 0; i < NUM_ATTRACTORS; i++)
    {
        StructAttractor attractor;

        attractor.mass      = masses[i];
        attractor.radius    = radiuses[i];
        attractor.density   = densities[i];
        attractor.position  = positions[i];

        attractors.push_back(attractor);
    }

    buffer_physics_attractors.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMemory_physics_attractors.resize(MAX_FRAMES_IN_FLIGHT);
    bufferMapped_physics_attractors.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            buffer_physics_attractors[i],
            bufferMemory_physics_attractors[i]
        );

        vkMapMemory(*pDevice, bufferMemory_physics_attractors[i], 0, bufferSize, 0, &bufferMapped_physics_attractors[i]);
        memcpy(bufferMapped_physics_attractors[i], attractors.data(), (size_t)bufferSize);
        vkUnmapMemory(*pDevice, bufferMemory_physics_attractors[i]);
    }

}

void Buffers::clearBuffers1(){

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(*pDevice, buffer_uniformMVP[i], nullptr);
        vkFreeMemory(*pDevice, bufferMemory_uniformMVP[i], nullptr);
    }
    for (size_t i = 0; i < buffer_storageTransformations.size(); i++) {
        vkDestroyBuffer(*pDevice, buffer_storageTransformations[i], nullptr);
        vkFreeMemory(*pDevice, bufferMemory_storageTransformations[i], nullptr);
    }
    for (size_t i = 0; i < buffer_uniformDeltaTime.size(); i++) {
        vkDestroyBuffer(*pDevice, buffer_uniformDeltaTime[i], nullptr);
        vkDestroyBuffer(*pDevice, buffer_physics_particles[i], nullptr);
        vkDestroyBuffer(*pDevice, buffer_physics_attractors[i], nullptr);
        vkDestroyBuffer(*pDevice, buffer_physics_constants[i], nullptr);

        vkFreeMemory(*pDevice, bufferMemory_uniformDeltaTime[i], nullptr);
        vkFreeMemory(*pDevice, bufferMemory_physics_particles[i], nullptr);
        vkFreeMemory(*pDevice, bufferMemory_physics_constants[i], nullptr);
        vkFreeMemory(*pDevice, bufferMemory_physics_attractors[i], nullptr);
    }
    
    vkDestroyBuffer(*pDevice, buffer_lbvh_particles, nullptr);
    vkDestroyBuffer(*pDevice, buffer_lbvh_elements, nullptr);
    vkDestroyBuffer(*pDevice, buffer_lbvh_mortonCode, nullptr);
    vkDestroyBuffer(*pDevice, buffer_lbvh_mortonCodePingPong, nullptr);
    vkDestroyBuffer(*pDevice, buffer_lbvh_LBVH, nullptr);
    vkDestroyBuffer(*pDevice, buffer_lbvh_LBVHConstructionInfo, nullptr);
    vkDestroyBuffer(*pDevice, buffer_lbvh_LBVH_host_vis, nullptr);

    vkFreeMemory(*pDevice, bufferMemory_lbvh_particles, nullptr);
    vkFreeMemory(*pDevice, bufferMemory_lbvh_elements, nullptr);
    vkFreeMemory(*pDevice, bufferMemory_lbvh_mortonCode, nullptr);
    vkFreeMemory(*pDevice, bufferMemory_lbvh_mortonCodePingPong, nullptr);
    vkFreeMemory(*pDevice, bufferMemory_lbvh_LBVH, nullptr);
    vkFreeMemory(*pDevice, bufferMemory_lbvh_LBVHConstructionInfo, nullptr);
    vkFreeMemory(*pDevice, bufferMemory_lbvh_LBVH_host_vis, nullptr);
}

void Buffers::clearBuffers2(){
    vkDestroyBuffer(*pDevice, indexBuffer, nullptr);
    vkDestroyBuffer(*pDevice, vertexBuffer, nullptr);
    vkDestroyBuffer(*pDevice, buffer_lines, nullptr);
    vkFreeMemory(   *pDevice, indexBufferMemory, nullptr);
    vkFreeMemory(   *pDevice, vertexBufferMemory, nullptr);
    vkFreeMemory(   *pDevice, bufferMemory_lines, nullptr);
}


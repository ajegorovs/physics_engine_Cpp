#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>
#include <iostream>
#include <fstream>
#include <array>
#include <cmath>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/euler_angles.hpp> 
#include "glm/ext.hpp"


//struct Particle {
//    glm::vec2 position;
//    glm::vec2 velocity;
//    glm::vec4 color;
//
//    static VkVertexInputBindingDescription getBindingDescription() {
//        VkVertexInputBindingDescription bindingDescription{};
//        bindingDescription.binding = 0;
//        bindingDescription.stride = sizeof(Particle);
//        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//        return bindingDescription;
//    }
//
//    // vertex attributes are only position and color.
//    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
//        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
//
//        attributeDescriptions[0].binding = 0;
//        attributeDescriptions[0].location = 0;
//        attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
//        attributeDescriptions[0].offset = offsetof(Particle, position);
//
//        attributeDescriptions[1].binding = 0;
//        attributeDescriptions[1].location = 1;
//        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//        attributeDescriptions[1].offset = offsetof(Particle, color);
//
//        return attributeDescriptions;
//    }
//};
//
//struct ParticleUniformBufferObject {
//    float deltaTime = 1.0f;
//};



struct point3D
{
    glm::vec4 color;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::float32 mass;
    glm::float32 damping;

    // these are used for vertex buffer.
    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0; // wrong
        bindingDescription.stride = sizeof(point3D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        // Populate the vector with attribute descriptions
        attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(point3D, color) });

        attributeDescriptions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(point3D, position) });

        attributeDescriptions.push_back({ 2,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(point3D, velocity) });

        attributeDescriptions.push_back({ 3,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(point3D, acceleration) });

        attributeDescriptions.push_back({ 4,0,VK_FORMAT_R32_SFLOAT,offsetof(point3D, mass) });

        attributeDescriptions.push_back({ 5,0,VK_FORMAT_R32_SFLOAT,offsetof(point3D, damping) });

        return attributeDescriptions;
    };

};

struct LineSegment3D
{
    glm::vec3 p1;
    glm::vec3 p2;

};
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsAndComputeFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsAndComputeFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 color;
    glm::vec2 texCoord;
    glm::float32 hasTex;
    glm::float32 objID;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        // Populate the vector with attribute descriptions
        attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, pos) });
     
        attributeDescriptions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(Vertex, color) });

        attributeDescriptions.push_back({ 2,0,VK_FORMAT_R32G32_SFLOAT,offsetof(Vertex, texCoord) });

        attributeDescriptions.push_back({ 3,0,VK_FORMAT_R32_SFLOAT,offsetof(Vertex, hasTex) });
   
        attributeDescriptions.push_back({ 4,0,VK_FORMAT_R32_SFLOAT,offsetof(Vertex, objID) });
           
        return attributeDescriptions;
    };

    bool operator==(const Vertex& other) const {
        return pos == other.pos && color == other.color && texCoord == other.texCoord && hasTex == other.hasTex;
    }
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1) ^ (hash<glm::float32>()(vertex.hasTex) << 1);
        }
    };
}


struct StructMVP {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct StructObjectTransformations {
    std::array<glm::mat4, 10> model;
};


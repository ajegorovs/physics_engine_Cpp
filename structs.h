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

#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/euler_angles.hpp> 
#include "glm/ext.hpp"

#include "config.h"
#include "physics.h"


struct StructDeltaTime
{
    glm::float32 deltaTime;
};

struct StructDeltaTimeLBVH
{
    glm::float32 deltaTime;
    uint32_t g_num_elements;
};


// #define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES should deal with glm::X
struct point3D {
    alignas(16) glm::vec4 color;         
    alignas(16) glm::vec3 position;      
    alignas(16) glm::vec3 velocity;      
    alignas(16) glm::vec3 acceleration;  
    alignas(16) glm::vec3 bbmin;  
    alignas(16) glm::vec3 bbmax;  
    alignas(4)  glm::float32 mass;       
    alignas(4)  glm::float32 damping;    
    alignas(4)  float group_id;          

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0; 
        bindingDescription.stride = sizeof(point3D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        
        std::vector<std::pair<size_t, VkFormat>> attributes = {
            { offsetof(point3D, color       ), VK_FORMAT_R32G32B32A32_SFLOAT },
            { offsetof(point3D, position    ), VK_FORMAT_R32G32B32_SFLOAT },
            { offsetof(point3D, velocity    ), VK_FORMAT_R32G32B32_SFLOAT },
            { offsetof(point3D, acceleration), VK_FORMAT_R32G32B32_SFLOAT },
            { offsetof(point3D, bbmin       ), VK_FORMAT_R32G32B32_SFLOAT },
            { offsetof(point3D, bbmax       ), VK_FORMAT_R32G32B32_SFLOAT },
            { offsetof(point3D, mass        ), VK_FORMAT_R32_SFLOAT },
            { offsetof(point3D, damping     ), VK_FORMAT_R32_SFLOAT },
            { offsetof(point3D, group_id    ), VK_FORMAT_R32_UINT }
        };
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
        attributeDescriptions.reserve(attributes.size());

        for (uint32_t i = 0; i < attributes.size(); i++)
        {
            size_t offset = attributes[i].first;
            VkFormat format = attributes[i].second;
            attributeDescriptions.push_back({ i ,0, format, static_cast<uint32_t>(offset) });
        }

        return attributeDescriptions;
    };

};

struct StructLineSegment3D
{   // 
    alignas(16) glm::vec3 position1;
    alignas(16) glm::vec3 position2;
    alignas(16) glm::vec4 color1;
    alignas(16) glm::vec4 color2;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(StructLineSegment3D);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(StructLineSegment3D, position1) });

        attributeDescriptions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(StructLineSegment3D, position2) });

        attributeDescriptions.push_back({ 2,0,VK_FORMAT_R32G32B32A32_SFLOAT,offsetof(StructLineSegment3D, color1) });

        attributeDescriptions.push_back({ 3,0,VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(StructLineSegment3D, color2) });

        return attributeDescriptions;
    };

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


struct VertexBase {
    alignas(16) glm::vec3 pos;
    alignas(16) glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexBase);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

        // Populate the vector with attribute descriptions
        attributeDescriptions.push_back({ 0,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(VertexBase, pos) });

        attributeDescriptions.push_back({ 1,0,VK_FORMAT_R32G32B32_SFLOAT,offsetof(VertexBase, color) });

        return attributeDescriptions;
    };
};

struct Vertex : public VertexBase{
    alignas(8) glm::vec2 texCoord;
    alignas(4) glm::float32 hasTex;
    alignas(4) glm::float32 objID;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions = VertexBase::getAttributeDescriptions();
        uint32_t binding = 0;
        uint32_t index = static_cast<uint32_t>(attributeDescriptions.size());
        std::vector<uint32_t> offsets = { offsetof(Vertex, texCoord), offsetof(Vertex, hasTex), offsetof(Vertex, objID) };
        std::vector <VkFormat> formats = { VK_FORMAT_R32G32_SFLOAT , VK_FORMAT_R32_SFLOAT , VK_FORMAT_R32_SFLOAT };
        for (uint32_t i = 0; i < offsets.size(); i++) {
            attributeDescriptions.push_back({ index + i, binding, formats[i], offsets[i] });
        }


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

struct GlobalBoundingBox {
    float minX;
    float minY;
    float minZ;
    float maxX;
    float maxY;
    float maxZ;
};

#define INVALID_POINTER 0x0

// input for the builder (normally a triangle or some other kind of primitive); it is necessary to allocate the buffer on the GPU
// and to upload the input data
struct Element {
    uint32_t primitiveIdx; // the id of the primitive; this primitive id is copied to the leaf nodes of the BVH (LBVHNode)
    float aabbMinX;        // aabb of the primitive
    float aabbMinY;
    float aabbMinZ;
    float aabbMaxX;
    float aabbMaxY;
    float aabbMaxZ;
    float mass;
};

// output of the builder; it is necessary to allocate the (empty) buffer on the GPU
struct LBVHNode {
    int32_t left;          // pointer to the left child or INVALID_POINTER in case of leaf
    int32_t right;         // pointer to the right child or INVALID_POINTER in case of leaf
    uint32_t primitiveIdx; // custom value that is copied from the input Element or 0 in case of inner node
    float aabbMinX;        // aabb of the node
    float aabbMinY;
    float aabbMinZ;
    float aabbMaxX;
    float aabbMaxY;
    float aabbMaxZ;
    float mass;
    float cx;
    float cy;
    float cz;
};

// only used on the GPU side during construction; it is necessary to allocate the (empty) buffer on the GPU
struct MortonCodeElement {
    uint32_t mortonCode; // key for sorting
    uint32_t elementIdx; // pointer into element buffer
};

// only used on the GPU side during construction; it is necessary to allocate the (empty) buffer on the GPU
struct LBVHConstructionInfo {
    uint32_t parent;         // pointer to the parent
    int32_t visitationCount; // number of threads that arrived
};

struct StructLBVH_NUM_ELEMENTS {
    uint32_t g_num_elements; // = NUM_ELEMENTS
};

struct PushConstantsHierarchy {
    uint32_t g_num_elements; // = NUM_ELEMENTS
    uint32_t g_absolute_pointers; // 1 or 0 (**)
};

struct PushConstantsBoundingBoxes {
    uint32_t g_num_elements; // = NUM_ELEMENTS
    uint32_t g_absolute_pointers; // 1 or 0 (**)
};
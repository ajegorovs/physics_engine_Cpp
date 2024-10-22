#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <optional>
#include <vector>
#include <iostream>
#include <fstream>
#include <array>
#include <cmath>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/euler_angles.hpp> 
#include "glm/ext.hpp"

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
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

    static std::array<VkVertexInputAttributeDescription, 5> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, hasTex);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(Vertex, objID);

        return attributeDescriptions;
    }

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

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};

struct StorageBufferObject {
    std::array<glm::mat4, 10> model;
};

class geometric_shape {
public:
    glm::vec3 center;
    std::string type;
    uint32_t id;
    
    glm::vec2 rotationZY;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec4> vertices;
    std::vector<int32_t> indices;

    geometric_shape(const std::string t, glm::vec3 c, glm::vec2 rotZY) : type(t), center(c), rotationZY(rotZY){}

    void transform(int numVerts, glm::vec2 rotationZYAngles, glm::vec3 sides) {
        glm::mat4 rotate = glm::mat4(1.0f);
        rotate = glm::rotate(rotate, rotationZYAngles.x, glm::vec3(0.0f, 1.0f, 0.0f));      // Yaw
        rotate = glm::rotate(rotate, rotationZYAngles.y, glm::vec3(1.0f, 0.0f, 0.0f));      // Pitch
        //rotate = glm::rotate(rotate, roll, glm::vec3(0.0f, 0.0f, 1.0f));

        //glm::mat4 rotate = glm::yawPitchRoll(rotationZYAngles.x, rotationZYAngles.y, glm::radians(0.0f));
        //auto rotate = glm::eulerAngleZY(rotationZYAngles.x, rotationZYAngles.y);
        auto scale = glm::diagonal4x4(glm::vec4(sides, 0));
        for (size_t i = 0; i < vertices.size(); i++) {
            // set colors to [0,1] range
            colors.push_back(glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z) + glm::vec3(0.5f, 0.5f, 0.5f));
            // rotate using eulerian X->Z angles (while prism is centeted), rescale and translate;
            vertices[i] = scale * rotate * vertices[i] + glm::vec4(center, 0.0f);
        }
    }
    virtual ~geometric_shape() = default;
};


class Prism : public geometric_shape {
public:
    glm::vec3 sides;

    Prism(glm::vec3 s, glm::vec3 c, glm::vec2 rotZY) : geometric_shape("Prism", c, rotZY), sides(s) {

        vertices.insert(vertices.end(), {{-0.5f, -0.5f, -0.5f, 0.f},{ 0.5f, -0.5f, -0.5f, 0.f},
                                        { 0.5f,  0.5f, -0.5f, 0.f}, {-0.5f,  0.5f, -0.5f, 0.f},
                                        {-0.5f, -0.5f,  0.5f, 0.f}, { 0.5f, -0.5f,  0.5f, 0.f},
                                        { 0.5f,  0.5f,  0.5f, 0.f}, {-0.5f,  0.5f,  0.5f, 0.f}});

        transform(vertices.size(), rotZY, sides);

        indices.insert(indices.end(), { 1,0,2,2,0,3, // order defines normal ? 
                                        4,5,6,6,7,4, // ? only outward part is rendered ? :(
                                        1,2,6,6,5,1,
                                        0,4,3,3,4,7,
                                        0,1,5,5,4,0,
                                        6,2,7,7,2,3 });
    }
};

class Cube : public Prism {
public:

    Cube(double s, glm::vec3 c, glm::vec2 rotZY) : Prism(glm::vec3(s, s, s), c, rotZY) {
        type = "Cube";
    }
};


class Plane : public geometric_shape {
public:
    glm::vec3 sides;
      Plane(glm::vec3 s, glm::vec3 c, glm::vec2 rotZY) : geometric_shape("Plane", c, rotZY), sides(s) {
        vertices.insert(vertices.end(), {
                                        {-0.5f, -0.5f, 0.0f, 0.f},
                                        {0.5f, -0.5f, 0.0f, 0.f},
                                        {0.5f, 0.5f, 0.0f, 0.f},
                                        {-0.5f, 0.5f, 0.0f, 0.f}
                                        });

        transform(vertices.size(), rotZY, sides);
        indices.insert(indices.end(), {0,1,2,2,3,0});

    }
};

//Prism prism(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0));
//Cube cube(1., glm::vec3(0, 0, 0), glm::vec2(0, 0));
//std::cout << cube.type;
//std::vector<std::unique_ptr<geometric_shape>> scene;
//scene.push_back(std::make_unique<Prism>(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec2(0, 0)));
//scene.push_back(std::make_unique<Cube>(1., glm::vec3(0, 0, 0), glm::vec2(0, 0)));
//auto prism_type = scene[0] -> type;*/
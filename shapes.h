#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <vector>
#include <string>

class geometric_shape {
public:
    glm::vec3 center;
    std::string type;
    uint32_t id;

    glm::vec2 rotationZY;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec4> vertices;
    std::vector<int32_t> indices;
    geometric_shape();
    geometric_shape(const std::string t, glm::vec3 c, glm::vec2 rotZY);

    void transform(int numVerts, glm::vec2 rotationZYAngles, glm::vec3 sides);
    virtual ~geometric_shape() = default;
};


class Prism : public geometric_shape {
public:
    glm::vec3 sides;

    Prism(glm::vec3 s, glm::vec3 c, glm::vec2 rotZY);
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
    Plane(glm::vec3 s, glm::vec3 c, glm::vec2 rotZY);
};




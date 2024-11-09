#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include "structs.h"

class Misc {
public:

	static std::vector<char> readFile(const std::string& filename);
    static VkShaderModule createShaderModule(VkDevice* pDevice, const std::vector<char>& code);
    static unsigned int expandBits(unsigned int v);
    static unsigned int morton3D(float x, float y, float z);
    static glm::vec3 rollSphereCoords(float r_min, float r_max, glm::vec3 rolls);
    static std::vector<glm::vec3> seedUniformPoints2D(const int N);
    static std::vector<glm::vec3> seedUniformGridPoints3D(const int N);
    static std::vector<glm::vec3> seedUniformSpherePoints3D(const int N);
    static std::vector<glm::vec3> sortByMorton(std::vector<glm::vec3> input);
    static std::vector<float> getExtent(std::vector<glm::vec3> points);
    static std::vector<MortonCodeElement> importFromCSV(const std::string& filename);
};  
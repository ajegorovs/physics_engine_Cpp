#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>

class Misc {
public:
	static std::vector<char> readFile(const std::string& filename);
    static VkShaderModule createShaderModule(VkDevice* pDevice, const std::vector<char>& code);
    static unsigned int expandBits(unsigned int v);
    static unsigned int morton3D(float x, float y, float z);
    static std::vector<glm::vec3> seedUniformPoints2D(const int N);
    static std::vector<glm::vec3> seedUniformGridPoints3D(const int N);
    static std::vector<glm::vec3> seedUniformSpherePoints3D(const int N);
    static std::vector<glm::vec3> sortByMorton(std::vector<glm::vec3> input);
    static std::vector<float> getExtent(std::vector<glm::vec3> points);
};
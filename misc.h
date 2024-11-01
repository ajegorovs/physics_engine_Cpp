#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class Misc {
public:
	static std::vector<char> readFile(const std::string& filename);
    static VkShaderModule createShaderModule(VkDevice* pDevice, const std::vector<char>& code);
    static unsigned int expandBits(unsigned int v);
    static unsigned int morton3D(float x, float y, float z);
    static std::vector<std::array<float, 3>> seedUniformPoints2D(const int N);
    static std::vector<std::array<float, 3>> sortByMorton(std::vector<std::array<float, 3>> input);
};
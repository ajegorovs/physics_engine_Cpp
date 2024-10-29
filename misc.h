#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class Misc {
public:
	static std::vector<char> readFile(const std::string& filename);
    static VkShaderModule createShaderModule(VkDevice* pDevice, const std::vector<char>& code);
    
};
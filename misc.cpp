#include "misc.h"
#include <fstream>  
#include <random>  
#include <array>  
#include <algorithm> 
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
//#include <iostream>


std::vector<char> Misc::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkShaderModule Misc::createShaderModule(VkDevice* pDevice, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(*pDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }

    return shaderModule;
}

unsigned int Misc::expandBits(unsigned int v)
{
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;
    return v;
}

unsigned int Misc::morton3D(float x, float y, float z)
{
    x = std::min(std::max(x * 1024.0f, 0.0f), 1023.0f);
    y = std::min(std::max(y * 1024.0f, 0.0f), 1023.0f);
    z = std::min(std::max(z * 1024.0f, 0.0f), 1023.0f);
    unsigned int xx = expandBits((unsigned int)x);
    unsigned int yy = expandBits((unsigned int)y);
    unsigned int zz = expandBits((unsigned int)z);
    return xx * 4 + yy * 2 + zz;
}

std::vector<std::array<float,3>> Misc::seedUniformPoints2D(const int N)
{
    std::vector<std::array<float, 3>> output;
    output.reserve(N);
    std::default_random_engine rndEngine((unsigned)time(nullptr));
    std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

    for (size_t i = 0; i < N; i++)
    {
        std::array<float, 3> p{ rndDist(rndEngine), rndDist(rndEngine), 0.1f * rndDist(rndEngine) };
        output.push_back(p);
    }
    return output;
}

#include <vector>
#include <array>
#include <algorithm>

// Assume Misc::morton3D function is defined elsewhere

std::vector<std::array<float, 3>> Misc::sortByMorton(std::vector<std::array<float, 3>> input)
{
    // Generate Morton code for each point and store them with their associated points
    std::vector<std::pair<unsigned int, std::array<float, 3>>> mortonPairs;
    mortonPairs.reserve(input.size());

    for (const auto& p : input)
    {
        mortonPairs.emplace_back(Misc::morton3D(p[0], p[1], p[2]), p);
    }

    // Sort the pairs by Morton code
    std::sort(mortonPairs.begin(), mortonPairs.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    // Extract sorted points
    std::vector<std::array<float, 3>> sortedInput;
    sortedInput.reserve(input.size());

    for (const auto& pair : mortonPairs)
    {
        sortedInput.push_back(pair.second);
    }

    return sortedInput;
}

#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <map>
#include "spirv_reflect.h"
#include "structs.h"


struct DescriptorSetLayoutData {
    uint32_t set_number{};
    VkDescriptorSetLayoutCreateInfo create_info{};
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    std::map<uint32_t, uint32_t> bindingToIndex;

    void generateBindingToIndexMap() {
        for (uint32_t i = 0; i < bindings.size(); i++) {
            const auto& binding = bindings[i];
            bindingToIndex[binding.binding] = i;
        }
    }
};

class Shader {
    VkDevice* pDevice;
public:

    Shader(VkDevice* pDevice);
    std::map<uint32_t, DescriptorSetLayoutData> m_descriptorSetLayoutData;
    VkShaderModule m_shaderModule;
    VkExtent3D m_workGroupSize = { 0, 0, 0 };

    static void compileShader(const std::string& fileName);
    void reflectDescriptorSetLayout(const SpvReflectShaderModule& module);
    void reflectWorkGroupSize(const SpvReflectShaderModule& module);
    void reflect(const std::vector<char>& code);
    void prepShader(const std::string& fileName);



};
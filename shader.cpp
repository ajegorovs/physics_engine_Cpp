#include "shader.h"
#include "misc.h"
//#include <fstream>  
#include <sstream>


std::string shaderPath = "./shaders";

Shader::Shader(VkDevice* pDevice) : pDevice(pDevice) {}

void Shader::compileShader(const std::string& fileName) {

    std::stringstream cmd;
    cmd << "glslc --target-spv=spv1.5 " << shaderPath << "/" << fileName << ".comp -o " << "./shaders/" << fileName << ".spv";

    std::cout << cmd.str() << std::endl;
    std::string cmd_output;
    char read_buffer[1024];
    FILE* cmd_stream = _popen(cmd.str().c_str(), "r");
    while (fgets(read_buffer, sizeof(read_buffer), cmd_stream))
        cmd_output += read_buffer;
    int cmd_ret = _pclose(cmd_stream);

    if (cmd_ret != 0) {
        throw std::runtime_error("unable to compile");
    }
}

void Shader::reflectDescriptorSetLayout(const SpvReflectShaderModule& module) {
    uint32_t count = 0;
    SpvReflectResult result = spvReflectEnumerateDescriptorSets(&module, &count, NULL);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    std::vector<SpvReflectDescriptorSet*> sets(count);
    result = spvReflectEnumerateDescriptorSets(&module, &count, sets.data());
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    for (const auto& reflectedSet : sets) {
        m_descriptorSetLayoutData[reflectedSet->set] = {};
        DescriptorSetLayoutData& layout = m_descriptorSetLayoutData[reflectedSet->set];

        layout.bindings.resize(reflectedSet->binding_count);
        for (uint32_t i_binding = 0; i_binding < reflectedSet->binding_count; ++i_binding) {
            const SpvReflectDescriptorBinding& refl_binding = *(reflectedSet->bindings[i_binding]);
            VkDescriptorSetLayoutBinding& layout_binding = layout.bindings[i_binding];
            layout_binding.binding = refl_binding.binding;
            layout_binding.descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);
            layout_binding.descriptorCount = 1;
            for (uint32_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim) {
                layout_binding.descriptorCount *= refl_binding.array.dims[i_dim];
            }
            layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(module.shader_stage);

            if (layout_binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                // reflect uniform
                //m_uniforms[reflectedSet->set].push_back(Uniform::reflect(m_gpuContext, refl_binding));
            }
        }
        layout.set_number = reflectedSet->set;
        layout.create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout.create_info.bindingCount = reflectedSet->binding_count;
        layout.create_info.pBindings = layout.bindings.data();
    }
}

void Shader::reflectWorkGroupSize(const SpvReflectShaderModule& module) {
    auto entryPoint = spvReflectGetEntryPoint(&module, "main");
    if (entryPoint != nullptr) {
        m_workGroupSize = { entryPoint->local_size.x, entryPoint->local_size.y, entryPoint->local_size.z };
    }
}

void Shader::reflect(const std::vector<char>& code) {
    SpvReflectShaderModule module = {};
    SpvReflectResult result =
        spvReflectCreateShaderModule(sizeof(code[0]) * code.size(), code.data(), &module);
    assert(result == SPV_REFLECT_RESULT_SUCCESS);

    reflectDescriptorSetLayout(module);
    reflectWorkGroupSize(module);

    spvReflectDestroyShaderModule(&module);
}

void Shader::prepShader( const std::string& fileName) {
    std::cout << "[Shader] (not used. testing) Compiling " << shaderPath << "/" << fileName << std::endl;

    compileShader(fileName);
    std::vector<char> code = Misc::readFile(shaderPath + "/" + fileName + ".spv");

    reflect(code);

    m_shaderModule = Misc::createShaderModule(pDevice, code);
}

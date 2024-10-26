#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "structs.h"

class Render {
public:
	VkRenderPass renderPass;
	void createRenderPass(VkFormat swapChainImageFormat, VkFormat findDepthFormat);
	void createDescriptorSetLayout_multi_MPV_TS_TRN(); // Swapchain->createDescriptorPool, createDescriptorSets_multi_MPV_TS_TRN
	void createDescriptorSetLayout_storageParticles();  // as vertex buffer
	void createDescriptorSetLayout_uniformMVP();		// MVP only
	Render();
	Render(VkDevice* device, VkSampleCountFlagBits* msaaSamples);
	VkDescriptorSetLayout descriptorSetLayout_multi_MPV_TS_TRN;
	VkDescriptorSetLayout descriptorSetLayout_uniformMVP;
	VkDescriptorSetLayout descriptorSetLayout_storageParticles;
	
	VkPipelineLayout pipelineLayout;
	VkPipelineLayout pipelineLayout_particles;
	//VkPipelineLayout pipelineLayout_uniformMVP;

	VkPipeline graphicsPipeline;
	VkPipeline graphicsPipeline_particles;
	//VkPipelineLayout computePipelineLayout;
	//VkPipeline computePipeline;
	void createGraphicsPipeline_storageVertices();
	void createGraphicsPipeline_storageParticles();
	void cleanup();
private:
	void createGraphicsPipeline(
		VkPipeline* graphicsPipeline, VkPrimitiveTopology topology,
		VkPipelineLayout* pipelineLayout, VkDescriptorSetLayout* descriptorSetLayout, std::string vertDir, std::string fragDir,
		VkVertexInputBindingDescription bindingDescription,
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions);
	VkDevice* device;
	VkSampleCountFlagBits* msaaSamples;
	VkShaderModule createShaderModule(const std::vector<char>& code);
	static std::vector<char> readFile(const std::string& filename);
};
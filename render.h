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
	void createDescriptorSetLayout(); // Swapchain->createDescriptorPool, createDescriptorSets
	//void createComputeDescriptorSetLayout();
	void createGraphicsPipeline();
	//void createParticleGraphicsPipeline();
	//void createComputePipeline();
	Render();
	Render(VkDevice* device, VkSampleCountFlagBits* msaaSamples);
	VkDescriptorSetLayout descriptorSetLayout;
	//VkDescriptorSetLayout computeDescriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	//VkPipelineLayout computePipelineLayout;
	//VkPipeline computePipeline;
	void cleanup();
private:
	VkDevice* device;
	VkSampleCountFlagBits* msaaSamples;
	VkShaderModule createShaderModule(const std::vector<char>& code);
	static std::vector<char> readFile(const std::string& filename);
};
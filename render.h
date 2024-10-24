#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "structs.h"

class Render {
public:
	VkRenderPass renderPass;
	void createRenderPass(VkFormat swapChainImageFormat, VkFormat findDepthFormat);
	void createDescriptorSetLayout();
	void createGraphicsPipeline();
	Render();
	Render(VkDevice* device, VkSampleCountFlagBits* msaaSamples);
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	void cleanup();
private:
	VkDevice* device;
	VkSampleCountFlagBits* msaaSamples;
	VkShaderModule createShaderModule(const std::vector<char>& code);
};
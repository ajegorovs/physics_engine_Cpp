#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>

class Graphics {
public:
	Graphics();
	Graphics(VkDevice* pDevice, VkSampleCountFlagBits* pMsaaSamples);
	VkPipelineLayout pipelineLayout;
	VkPipelineLayout pipelineLayout_particles;
	VkPipelineLayout pipelineLayout_lines;

	VkPipeline graphicsPipeline;
	VkPipeline graphicsPipeline_particles;
	VkPipeline graphicsPipeline_lines;

	void createGraphicsPipeline_storageVertices(VkDescriptorSetLayout* pDescriptorSetLayout, VkRenderPass* pRenderPass);
	void createGraphicsPipeline_storageLines(VkDescriptorSetLayout* pDescriptorSetLayout, VkRenderPass* pRenderPass);
	void createGraphicsPipeline_storageParticles(VkDescriptorSetLayout* pDescriptorSetLayout, VkRenderPass* pRenderPass);

	void cleanup();

private:
	VkDevice* pDevice;
	VkSampleCountFlagBits* pMsaaSamples;

	void createGraphicsPipeline(
		VkPipeline* pGraphicsPipeline, VkPrimitiveTopology topology,
		VkPipelineLayout* pPipelineLayout, VkDescriptorSetLayout* pDescriptorSetLayout, std::string vertDir, std::string fragDir,
		VkVertexInputBindingDescription bindingDescription,
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions, VkRenderPass* pRenderPass);




};
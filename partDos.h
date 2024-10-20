#pragma once
#include "config.h"


class PartDos {
public:
	void createSwapChain();
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	GLFWwindow* window;
	VkDevice device;
	VkSurfaceKHR surface;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	void init(GLFWwindow* window, VkDevice device, VkPhysicalDevice physicalDevice,  VkSurfaceKHR surface, VkSampleCountFlagBits msaaSamples);
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	void createImageViews();
	VkRenderPass renderPass;
	void createRenderPass();
	VkDescriptorSetLayout descriptorSetLayout;
	void createDescriptorSetLayout();
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	void createGraphicsPipeline();
private:
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
};
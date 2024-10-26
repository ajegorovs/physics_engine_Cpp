#pragma once
#define GLFW_INCLUDE_VULKAN	// VK_
#include <GLFW/glfw3.h>
//#include "structs.h"
#include <vector>		// std::vector
#include <cstdint>		// uint32_t 
#include "commands.h"	// Commands
#include "config.h"		// particles
/// <summary>
/// Comments:
/// cannot link GLFWwindow* window at constructor because its not init at that time at GLFW
/// </summary>
class Swapchain {
public:
	VkSurfaceKHR* surface;
	VkDevice* device;
	VkPhysicalDevice* physicalDevice;
	VkSampleCountFlagBits* msaaSamples;
	VkSwapchainKHR swapChain; //drawFrame
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	Swapchain();
	Swapchain(VkSurfaceKHR* surface, VkDevice* device, VkPhysicalDevice* physicalDevice, VkSampleCountFlagBits* msaaSamples);
	void createSwapChain(GLFWwindow* window);

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void createColorResources();
	void createDepthResources(VkFormat depthFormat);
	void createFramebuffers(VkRenderPass renderPass);
	void createTextureImage(Commands cmd, VkQueue graphicsQueue);
	void createTextureImageView();
	void createTextureSampler();
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets_multi_MPV_TS_TRN;
	std::vector<VkDescriptorSet> descriptorSets_storageParticles;
	std::vector<VkDescriptorSet> descriptorSets_uniformMVP;
	//std::vector<VkDescriptorSet> computeDescriptorSets;
	void createDescriptorPool(); // Render-> createDescriptorSetLayout_multi_MPV_TS_TRN
	void createDescriptorSets_multi_MPV_TS_TRN(VkDescriptorSetLayout descriptorSetLayout, std::vector<VkBuffer> uniformBuffers, std::vector<VkBuffer> storageBuffer);
	void createDescriptorSets_single(
		VkDescriptorSetLayout* descriptorSetLayout,
		std::vector<VkDescriptorSet>& descriptionSet,
		std::vector<VkBuffer>& buffer,
		unsigned long long range,
		int binding,
		VkDescriptorType descriptorType
		);
	void createDescriptorSets_storageParticles(VkDescriptorSetLayout* descriptorSetLayout, std::vector<VkBuffer>& storageBuffer);
	void createDescriptorSets_uniformMVP(VkDescriptorSetLayout* descriptorSetLayout, std::vector<VkBuffer>& storageBuffer);
	//void createComputeDescriptorSets(VkDescriptorSetLayout computeDescriptorSetLayout, std::vector<VkBuffer> uniformBuffers, std::vector<VkBuffer> shaderStorageBuffers);
	void cleanupSwapChain();
	void cleanupRest();
	
public:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	void createImageViews();
	static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	uint32_t mipLevels;

	void copyBufferToImage(Commands cmd, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void generateMipmaps(Commands cmd, VkQueue graphicsQueue, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

};
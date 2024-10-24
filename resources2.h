#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <cstdint>
#include "commands.h"

class Resources {
public:
	Resources();
	Resources(VkDevice* device, VkSurfaceKHR* surface, VkPhysicalDevice* physicalDevice, VkSampleCountFlagBits* msaaSamples);
	static std::vector<char> readFile(const std::string& filename);
	//VkCommandPool commandPool;
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	//void createCommandPool();
	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void createColorResources(VkFormat swapChainImageFormat, VkExtent2D swapChainExtent);
	void createDepthResources(VkFormat depthFormat, VkExtent2D swapChainExtent);
	void createFramebuffers(std::vector<VkFramebuffer>& swapChainFramebuffers, const std::vector<VkImageView>& swapChainImageViews, VkRenderPass renderPass, VkExtent2D swapChainExtent);
	void createTextureImage(Commands cmd, VkQueue graphicsQueue);
	void createTextureImageView();
	void createTextureSampler();
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	void createDescriptorPool();
	void createDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, std::vector<VkBuffer> uniformBuffers, std::vector<VkBuffer> storageBuffers);
	void cleanupSwapChain1();
	void cleanup();


private:
	VkDevice* device;
	VkSurfaceKHR* surface;
	VkPhysicalDevice* physicalDevice;
	VkSampleCountFlagBits* msaaSamples;

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
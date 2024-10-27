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

	VkRenderPass renderPass;
	VkSwapchainKHR swapChain;
	VkExtent2D swapChainExtent;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkImageView textureImageView;
	VkSampler textureSampler;

	Swapchain();
	Swapchain(VkSurfaceKHR* surface, VkDevice* pDevice, VkPhysicalDevice* physicalDevice, VkSampleCountFlagBits* msaaSamples);
	void createSwapChain(GLFWwindow* window);
	void createImageViews();
	void createRenderPass(VkFormat findDepthFormat);
	void createColorResources();
	void createDepthResources(VkFormat depthFormat);
	void createFramebuffers();
	void createTextureImage(Commands cmd, VkQueue graphicsQueue);
	void createTextureImageView();
	void createTextureSampler();


	void cleanupSwapChain();
	void cleanupRenderPass();
	void cleanupRest();
	
private:
	VkSurfaceKHR* surface;
	VkDevice* pDevice;
	VkPhysicalDevice* physicalDevice;
	VkSampleCountFlagBits* msaaSamples;

	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	

	uint32_t mipLevels;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	std::vector<VkImageView> swapChainImageViews;

	void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	static VkImageView createImageView(VkDevice* pDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	
	void copyBufferToImage(Commands cmd, VkQueue graphicsQueue, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void generateMipmaps(Commands cmd, VkQueue graphicsQueue, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

};
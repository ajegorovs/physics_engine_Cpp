#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "structs.h"
/// <summary>
/// Comments:
/// cannot link GLFWwindow* window at constructor because its not init at that time at GLFW
/// </summary>
class Swapchain {
public:
	VkSurfaceKHR* surface;
	VkDevice* device;
	VkPhysicalDevice* physicalDevice;
	VkSwapchainKHR swapChain; //drawFrame
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	Swapchain();
	Swapchain(VkSurfaceKHR* surface, VkDevice* device, VkPhysicalDevice* physicalDevice);
	void createSwapChain(GLFWwindow* window);
	
public:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities);
	void createImageViews();
	static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void cleanupSwapChain2();

};
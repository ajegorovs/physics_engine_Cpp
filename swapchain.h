#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "structs.h"

class Swapchain {
public:
	VkSurfaceKHR* surface;
	VkDevice* device;
	VkPhysicalDevice* physicalDevice;
	GLFWwindow* window;
	VkSwapchainKHR swapChain; //drawFrame
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	Swapchain(VkSurfaceKHR* surface, VkDevice* device, VkPhysicalDevice* physicalDevice, GLFWwindow* window);
	void createSwapChain();
public:
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	void createImageViews();
	static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);

};
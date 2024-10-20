#pragma once
#include "config.h"

class PartUno {
public:
	VkInstance instance;
	VkSurfaceKHR surface;
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	bool validationLayerSupport;
	std::vector < const char*> extensions;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	void createInstance();
	void setupDebugMessenger();
	GLFWwindow* window;
	void createSurface();
	void pickPhysicalDevice();
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	void init(GLFWwindow* window);
	void destroy();
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	void createLogicalDevice();

private:
	VkDebugUtilsMessengerEXT debugMessenger;
	bool checkValidationLayerSupport();
	std::vector<const char*> getRequiredExtensions();
	VkSampleCountFlagBits getMaxUsableSampleCount();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
};

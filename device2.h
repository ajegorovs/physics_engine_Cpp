#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "config.h"
#include "structs.h"
#include <vector>
#include <cstdint>

// Logic: Device2 keeps strictiyl device related stuff. it uses pointers for higher stuff.
class Device2 {
private:
	VkInstance* pInstance;
	VkSurfaceKHR* pSurface;
	VkDevice* pDevice;
	const std::vector<const char*> deviceExtensions = { 
		VK_KHR_SWAPCHAIN_EXTENSION_NAME, 
		//VK_EXT_SHADER_ATOMIC_FLOAT_2_EXTENSION_NAME,
		"VK_EXT_extended_dynamic_state",  
		"VK_EXT_extended_dynamic_state2",  
		"VK_EXT_extended_dynamic_state3" ,
		"VK_EXT_shader_object"
	};
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice physicalDevice);
	QueueFamilyIndices findQueueFamilies();
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice physicalDevice);
	VkSampleCountFlagBits getMaxUsableSampleCount();

public:
	VkPhysicalDevice physicalDevice;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	Device2();
	Device2(VkInstance* pInstance, VkSurfaceKHR* pSurface, VkDevice* pDevice);
	static QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
	static SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
	static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void pickPhysicalDevice();
	void createLogicalDevice(const std::vector<const char*> validationLayers, VkQueue* pGraphicsQueue, VkQueue* pPresentQueue, VkQueue* pComputeQueue);
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();

};

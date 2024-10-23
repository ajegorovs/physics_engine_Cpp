#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "structs.h"
#include <set>

bool enableValidationLayers = true;

// Logic: Device2 keeps strictiyl device related stuff. it uses pointers for higher stuff.
class Device2 {
private:
	VkInstance* instance;
	VkSurfaceKHR* surface;
	VkDevice* device;
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool isDeviceSuitable(VkPhysicalDevice device);
	QueueFamilyIndices findQueueFamilies();
	SwapChainSupportDetails querySwapChainSupport();
	VkSampleCountFlagBits getMaxUsableSampleCount();

public:
	VkPhysicalDevice physicalDevice;
	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	Device2(VkInstance* instance, VkSurfaceKHR* surface, VkDevice* device);
	static QueueFamilyIndices findQueueFamilies(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
	static SwapChainSupportDetails querySwapChainSupport(VkSurfaceKHR surface, VkPhysicalDevice physicalDevice);
	static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void pickPhysicalDevice();
	void createLogicalDevice(const std::vector<const char*> validationLayers, VkQueue graphicsQueue, VkQueue presentQueue);

};

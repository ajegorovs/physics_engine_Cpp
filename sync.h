#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "config.h"
#include <vector>

class Sync {
public:
	VkDevice* pDevice;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkSemaphore> computeFinishedSemaphores;

	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> computeInFlightFences;
	Sync();
	Sync(VkDevice* pDevice);
	static void transitionImageLayout(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels);
	void createSyncObjects();
	void cleanup();
};
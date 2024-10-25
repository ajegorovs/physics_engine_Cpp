#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "config.h"
#include "device2.h"
#include "debug2.h"
#include "glfw_support.h"
#include "swapchain.h"
#include "render.h"
//#include "resources2.h"
#include "buffers.h"
#include "commands.h"
#include "sync.h"

class Engine {
public:
	Engine();
	VkInstance instance;
	VkSurfaceKHR surface;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkQueue computeQueue;
	void createInstance();
	void run();
	
private:
	GLFW_support glfw_s;
	Debug2 dbg;
	Device2 dvc;
	Swapchain swp;
	Render rndr;
	//Resources res;
	Buffers bfr;
	Commands cmd;
	Sync sync;
	uint32_t currentFrame = 0;
	//float lastFrameTime = 0.0f;
	//double lastTime = 0.0f;
	//uint32_t num_frames = 0;
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	void updateUniformBuffer(uint32_t currentImage);
	//void updateParticleUniformBuffer(uint32_t currentImage);
	void updateStorageBuffer(uint32_t currentImage);
	//void updateShaderStorageBuffer(uint32_t currentImage);
	void drawFrame();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	//void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);
	void recreateSwapChain();
	void cleanup();
};


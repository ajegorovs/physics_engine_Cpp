#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "config.h"
#include "device2.h"
#include "debug2.h"
#include "glfw_support.h"
#include "swapchain.h"
#include "graphics.h"
#include "compute.h"
#include "descriptors.h"
#include "sync.h"
#include "buffers.h"
#include "commands.h"
#include "sync.h"
#include "lbvh.h"
#include "shader.h"

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
	double lastFrameTime = 0.0f;
	double lastTime = 0.0f;
	bool recalculateLBVH;
	bool recalculateBBs;
	uint32_t cnt;
	PushConstantsMortonCodes pushConstMC{ NUM_ELEMENTS, -10.0f*P_R , -10.0f * P_R, -10.0f * P_R , 1.0f + 10.0f * P_R , 1.0f + 10.0f * P_R , 1.0f + 10.0f * P_R };

	
private:
	GLFW_support glfw_s;
	Debug2 dbg;
	Device2 dvc;
	Swapchain swp;
	Graphics gfx;
	Compute cmpt;
	Descriptors dscr;
	Buffers bfr;
	Commands cmd;
	Sync sync;
	Shader shdr;
	bool gotAtleatOneLVBH = false;
	uint32_t currentFrame = 0;
	PFN_vkCmdSetPrimitiveTopologyEXT vkCmdSetPrimitiveTopologyEXT = nullptr;
	
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
	void updateBufferMapped_uniformMVP(uint32_t currentImage);
	//void updateParticleUniformBuffer(uint32_t currentImage);
	void updateBufferMapped_storageTransformtions(uint32_t currentImage);
	//void updateBufferMapped_storageParticles(uint32_t currentImage);
	void updateBufferMapped_uniformDeltaTime(uint32_t currentImage);
	//void updateShaderStorageBuffer(uint32_t currentImage);
	void drawFrame();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);
	void recordLBVHComputeCommandBuffer(VkCommandBuffer commandBuffer, bool recalculate);
	void recordLBVHComputeCommandBuffer2(VkCommandBuffer commandBuffer, bool recalculate);
	//void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);
	void recreateSwapChain();
	void cleanup();
	void traverse(uint32_t index, LBVHNode* LBVH, std::vector<int>& visited);

	std::vector<float> masses;         
	std::vector<float> radiuses;       
	std::vector<float> densities;      
	std::vector<glm::vec3> positions;

};


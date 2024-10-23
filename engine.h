#pragma once
#include "memory.h"
#include "device2.h"
#include "debug2.h"
#include "glfw_support.h"
#include "swapchain.h"


class Engine {
public:
	Engine();
	VkInstance instance;
	VkSurfaceKHR surface;
	VkDevice device;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	void createInstance();
	void run();
private:
	Device2 dvc;
	Debug2 dbg;
	GLFW_support glfw_s;
	//Swapchain swp; inside run()
	const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

};
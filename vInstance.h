#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 
#include "vDebug.h"
#include "vDevice.h"


class VInst {
public:
	VkInstance instance;
	void createInstance(VDevice& vDevice, VDebug& vDebug);
};
#pragma once
#include "vApp.h"		// class

void VApp::run() {
	initWindow();
	p1.init(window);			// share window pointer with p1 + other
	p1.createInstance();		// instance pointer
	p1.setupDebugMessenger();	// debugMessenger pointer
	p1.createSurface();			// surface pointer
	p1.pickPhysicalDevice();	// physicalDevice, msaaSamples
	p1.createLogicalDevice();
	p2.init(window, p1.device, p1.physicalDevice, p1.surface, p1.msaaSamples);
	p2.createSwapChain();
	p2.createImageViews();
	p2.createRenderPass();
	p2.createDescriptorSetLayout();
	p2.createGraphicsPipeline();
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		//std::cout << framebufferResized << ")\n";
	}
	
	glfwDestroyWindow(window);

	glfwTerminate();
}

void VApp::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}


// Static callback function
void VApp::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<VApp*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true; // Set the framebufferResized flag
}

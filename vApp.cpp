#include "vApp.h"		// class
#include <GLFW/glfw3.h> // everything glfw
#include "config.h"		// WIDTH, HEIGHT
#include "vInstance.h"
#include "vDevice.h"
//#include "vDebug.h"
#include <iostream>




void VApp::run() {
	VApp::initWindow();
	VInst vInst;
	VDevice vDevice;
	VDebug vDebug;
	vInst.createInstance(vDevice, vDebug);
	vDebug.setupDebugMessenger(vInst.instance);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		std::cout << framebufferResized << ")\n";
	}
	//vkDestroyInstance(instance, nullptr);
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

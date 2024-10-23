#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

bool enableValidationLayers = true;

class GLFW_support {
public:
	GLFWwindow* window;
	void initWindow();
	static std::vector<const char*> getRequiredExtensions();
	void createSurface(VkInstance instance, VkSurfaceKHR surface);
	bool framebufferResized = false;
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

};
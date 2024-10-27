#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>

class GLFW_support {
public:
	GLFWwindow* window;
	GLFW_support();
	void initWindow();
	static std::vector<const char*> getRequiredExtensions();
	void createSurface(VkInstance instance, VkSurfaceKHR* pSurface);
	bool framebufferResized = false;
private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

};
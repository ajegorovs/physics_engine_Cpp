#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h> 
#include <vector>



class VDebug {
public:
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};
	bool checkValidationLayerSupport();
	VkDebugUtilsMessengerEXT debugMessenger;
	//void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger(VkInstance& instance);
	//VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);*/

};

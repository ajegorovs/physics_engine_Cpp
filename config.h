#pragma once
#define GLM_FORCE_CXX17
#include <iostream>
#include <vector>
#include <array>
#include <string>

// Window dimensions
const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;




const std::string MODEL_PATH = "models/viking_room.obj";
const std::string TEXTURE_PATH = "textures/viking_room.png";

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

const std::vector<const char*> filterValidationErrors = { 
	"VUID-vkCmdSetPrimitiveTopology-None-08971" ,
	//"VUID-VkImageViewCreateInfo-usage-02275",
	//"VUID-VkSwapchainCreateInfoKHR-imageFormat-01778"
};


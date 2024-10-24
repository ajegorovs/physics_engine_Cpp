#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include "structs.h"
#include "shapes.h"

class Buffers {
public:
	VkDevice* device;
	VkPhysicalDevice* physicalDevice;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkBuffer> storageBuffers;

	Buffers();
	Buffers(VkDevice* device, VkPhysicalDevice* physicalDevice);
	static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void processScene(const std::vector<std::unique_ptr<geometric_shape>>& scene);
	void createVertexBuffer(VkCommandPool commandPool, VkQueue graphicsQueue);
	void createIndexBuffer(VkCommandPool commandPool, VkQueue graphicsQueue);
	void createUniformBuffers();
	void createStorageBuffers();
	std::vector<void*> uniformBuffersMapped;
	std::vector<void*> storageBuffersMapped;
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	void clearBuffers1();
	void clearBuffers2();
private:
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<VkDeviceMemory> storageBuffersMemory;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;


};
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>		// std::vector
#include "structs.h"
#include "shapes.h"
#include <memory>		// std::unique_ptr

class Buffers {
public:
	VkDevice* device;
	VkPhysicalDevice* physicalDevice;
	VkCommandPool* commandPool;
	VkQueue* graphicsQueue;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<VkBuffer> uniformBuffers;
	//std::vector<VkBuffer> particleUniformBuffers;
	std::vector<VkBuffer> storageBuffers;
	//std::vector<VkBuffer> shaderStorageBuffers;

	Buffers();
	Buffers(VkDevice* device, VkPhysicalDevice* physicalDevice, VkCommandPool* commandPool, VkQueue* graphicsQueue);
	static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void processScene(const std::vector<std::unique_ptr<geometric_shape>>& scene);
	void createVertexBuffer();
	void createIndexBuffer();
	void createUniformBuffers();
	//void createParticleUniformBuffers();
	void createStorageBuffers();
	//void createShaderStorageBuffers();
	std::vector<void*> uniformBuffersMapped;
	//std::vector<void*> particleUniformBuffersMapped;
	std::vector<void*> storageBuffersMapped;
	//std::vector<void*> shaderStorageBuffersMapped;
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	void clearBuffers1();
	void clearBuffers2();
private:
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	std::vector<VkDeviceMemory> uniformBuffersMemory;
	//std::vector<VkDeviceMemory> particleUniformBuffersMemory;
	std::vector<VkDeviceMemory> storageBuffersMemory;
	//std::vector<VkDeviceMemory> shaderStorageBuffersMemory;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;


};
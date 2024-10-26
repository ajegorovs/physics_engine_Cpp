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
	std::vector<VkBuffer> buffer_storageTransformations;
	std::vector<VkBuffer> buffer_storageParticles;
	std::vector<VkBuffer> buffer_uniformMVP;

	Buffers();
	Buffers(VkDevice* device, VkPhysicalDevice* physicalDevice, VkCommandPool* commandPool, VkQueue* graphicsQueue);
	static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void processScene(const std::vector<std::unique_ptr<geometric_shape>>& scene);
	void createVertexBuffer();
	void createIndexBuffer();

	void createBuffer_storageTransformations();
	void createBuffer_storageParticles();
	void createBuffer_uniformMVP();

	std::vector<void*> bufferMapped_storageTransformtions;
	std::vector<void*> bufferMapped_storageParticles;
	std::vector<void*> bufferMapped_uniformMVP;
	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	void clearBuffers1();
	void clearBuffers2();
private:
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	std::vector<VkDeviceMemory> uniformBuffersMemory;
	std::vector<VkDeviceMemory> bufferMemory_storageTransformations;
	std::vector<VkDeviceMemory> bufferMemory_storageParticles;
	std::vector<VkDeviceMemory> bufferMemory_uniformMVP;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;


};
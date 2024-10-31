#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>		// std::vector
#include <array>
#include "structs.h"
#include "shapes.h"
#include <memory>		// std::unique_ptr

class Buffers {
public:
	VkDevice* pDevice;
	VkPhysicalDevice* physicalDevice;
	VkCommandPool* commandPool;
	VkQueue* graphicsQueue;
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<VertexBase> line_vertices;
	VkBuffer buffer_lines;
	std::vector<VkBuffer> buffer_storageTransformations;
	std::vector<VkBuffer> buffer_uniformMVP;
	std::vector<VkBuffer> buffer_uniformDeltaTime;
	std::vector<VkBuffer> buffer_physics_particles;
	std::vector<VkBuffer> buffer_physics_attractors;
	std::vector<VkBuffer> buffer_physics_constants;

	Buffers();
	Buffers(VkDevice * pDevice, VkPhysicalDevice* physicalDevice, VkCommandPool* commandPool, VkQueue* graphicsQueue);
	static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void processScene(const std::vector<std::unique_ptr<geometric_shape>>& pScene);
	void processGrid();
	void createVertexBuffer();
	void createBuffer_line();
	void createIndexBuffer();

	void createBuffer_storageTransformations();
	//void createBuffer_storageParticles(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::float32 grav_const, glm::vec3 reference_axis);
	void createBuffer_uniformMVP();

	void createBuffer_uniformDeltaTime();
	void createBuffer_physics_particles_compute();
	void createBuffer_physics_particles_constants();
	void createBuffer_physics_attractors(
		std::vector<float> masses, 
		std::vector<float> radiuses, 
		std::vector<float> densities, 
		std::vector<glm::vec3> positions);

	std::vector<void*> bufferMapped_storageTransformtions;
	std::vector<void*> bufferMapped_uniformMVP;
	std::vector<void*> bufferMapped_uniformDeltaTime;
	std::vector<void*> bufferMapped_physics_attractors;

	VkBuffer vertexBuffer;
	VkBuffer indexBuffer;
	void clearBuffers1();
	void clearBuffers2();
private:
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	std::vector<VkDeviceMemory> bufferMemory_storageTransformations;
	std::vector<VkDeviceMemory> bufferMemory_uniformMVP;
	std::vector<VkDeviceMemory> bufferMemory_uniformDeltaTime;
	std::vector<VkDeviceMemory> bufferMemory_physics_particles;
	std::vector<VkDeviceMemory> bufferMemory_physics_constants;
	std::vector<VkDeviceMemory> bufferMemory_physics_attractors;

	VkDeviceMemory bufferMemory_lines;
	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;


};
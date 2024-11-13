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
	//std::vector<VertexBase> line_vertices;
	VkBuffer buffer_lines;

	std::vector<MortonCodeElement> MC;

	std::vector<VkBuffer> buffer_storageTransformations;
	std::vector<VkBuffer> buffer_uniformMVP;
	std::vector<VkBuffer> buffer_uniformDeltaTime;
	std::vector<VkBuffer> buffer_physics_particles;
	std::vector<VkBuffer> buffer_physics_attractors;
	std::vector<VkBuffer> buffer_physics_constants;

	VkBuffer buffer_lbvh_particles;
	VkBuffer buffer_lbvh_particles_host_vis;
	VkBuffer buffer_lbvh_mortonCode;
	VkBuffer buffer_lbvh_mortonCode_host_vis;
	VkBuffer buffer_lbvh_mortonCode_host_vis2;
	VkBuffer buffer_lbvh_mortonCodePingPong;
	VkBuffer buffer_lbvh_LBVH;
	VkBuffer buffer_lbvh_LBVHConstructionInfo;
	VkBuffer buffer_lbvh_LBVH_host_vis;
	VkBuffer buffer_lbvh_global_BBs;
	VkBuffer buffer_lbvh_global_BBs_host_vis;

	VkDeviceMemory bufferMemory_lbvh_LBVH_host_vis;
	VkDeviceMemory bufferMemory_lbvh_particles_host_vis;
	VkDeviceMemory bufferMemory_lbvh_mortonCode_host_vis;
	VkDeviceMemory bufferMemory_lbvh_mortonCode_host_vis2;
	VkDeviceMemory bufferMemory_lbvh_global_BBs_host_vis;


	Buffers();
	Buffers(VkDevice * pDevice, VkPhysicalDevice* physicalDevice, VkCommandPool* commandPool, VkQueue* graphicsQueue);
	static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createBufferDeviceLocalData(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		std::vector<VkBuffer>& buffer,
		std::vector<VkDeviceMemory>& bufferMemory,
		const void* ptr
	);

	void createBufferDeviceLocalData(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		std::vector<VkBuffer>& buffer,
		std::vector<VkDeviceMemory>& bufferMemory
	);

	void createBufferDeviceLocalData(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory,
		const void* pStagingMem,
		VkBuffer& stagingBuffer,
		const void* pData
	);

	void createBufferDeviceLocalData(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory,
		const void* ptr
	);

	void createBufferDeviceLocalData(
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory
	);
	void processScene(const std::vector<std::unique_ptr<geometric_shape>>& pScene);
	//void processGrid();
	//void processMortonLines(std::vector<std::array<float, 3>> points, glm::vec3 color, glm::vec3 offset);
	void createVertexBuffer();
	void createBuffer_line();
	void createIndexBuffer();

	void createBuffer_storageTransformations();
	//void createBuffer_storageParticles(glm::vec3 mass_center_pos, glm::float32 bigMass, glm::float32 grav_const, glm::vec3 reference_axis);
	void createBuffer_uniformMVP();

	void createBuffer_lbvh_points(std::vector<glm::vec3> points, glm::vec3 color);
	void createBuffer_lbvh_points_host_vis();
	void createBuffer_lbvh_points_2sphere();
	void createBuffer_lbvh_points_rot_sphere();
	void createBuffer_lbvh_mortonCode();
	void createBuffer_lbvh_mortonCode_host_vis();
	void createBuffer_lbvh_mortonCodePingPong();
	void createBuffer_lbvh_LBVH();
	void createBuffer_lbvh_LBVH_hist_vis();
	void createBuffer_lbvh_LBVHConstructionInfo();
	void createBuffer_lbvh_global_BBs();
	void createBuffer_lbvh_get_init_BBs();


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

	void* bufferMapped_lines;
	void* bufferMapped_lbvh_LBVH_hist_vis;
	void* bufferMapped_lbvh_particles_host_vis;
	void* bufferMapped_lbvh_mortonCode_host_vis;
	void* bufferMapped_lbvh_mortonCode_host_vis2;
	void* bufferMapped_lbvh_global_BBs_host_vis;

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

	VkDeviceMemory bufferMemory_lbvh_particles;
	VkDeviceMemory bufferMemory_lbvh_mortonCode;
	VkDeviceMemory bufferMemory_lbvh_mortonCodePingPong;
	VkDeviceMemory bufferMemory_lbvh_LBVH;
	VkDeviceMemory bufferMemory_lbvh_LBVHConstructionInfo;
	VkDeviceMemory bufferMemory_lbvh_global_BBs;

	VkDeviceMemory vertexBufferMemory;
	VkDeviceMemory indexBufferMemory;
	VkDeviceMemory bufferMemory_lines;

	std::vector<point3D> points_lbvh;
	std::array<glm::vec3,2> lbvh_BB;
};
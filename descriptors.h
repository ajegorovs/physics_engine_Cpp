#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <array>

// * Descriptor Set Layout (DSL) determines what kind of resources will be available for stages of some pipeline.
// You specify number of bindings, their type and which stages they are available to.
// You create a pipeline using DSL, but then can swap resources, if they conform to DSL.
// * Descriptor Set (DS) defines a particular case/selection of resources that meet DSL requirements
// DS is what you swap. 

class Descriptors {
public:
	Descriptors();
	Descriptors(VkDevice * pDevice);

	void createDescriptorPool();

	void createDSL_multi_MPV_TS_TRN(); // Swapchain->createDescriptorPool, createDS_multi_MPV_TS_TRN
	void createDSL_uniformMVP();		// MVP only
	void createDSL_1UC_4SC();
	void createDSL_lbvh();

	void createDS_multi_MPV_TS_TRN(std::vector<VkBuffer> uniformBuffers, std::vector<VkBuffer> storageBuffer, VkImageView textureImageView, VkSampler textureSampler);
	void createDS_uniformMVP(std::vector<VkBuffer>& pUniformBuffer);


	void createDS_physics_compute(
		std::vector<VkBuffer>& pUniformBuffer,  // deltaTime
		std::vector<VkBuffer>& pStorageBuffer,  // constants
		std::vector<VkBuffer>& pStorageBuffer2, // attractors
		std::vector<VkBuffer>& pStorageBuffer3  // particle old/new
	);
	void createDS_lbvh(
		VkBuffer& bufferElements,  
		VkBuffer& bufferMortonCode,
		VkBuffer& bufferMortonCodePingPong,
		VkBuffer& bufferLBVH,
		VkBuffer& bufferLBVHConstructionInfo
	);

	void cleanupDSL();
	void cleanupDPool();

	VkDescriptorPool descriptorPool;

	VkDescriptorSetLayout descriptorSetLayout_multi_MPV_TS_TRN;
	VkDescriptorSetLayout descriptorSetLayout_uniformMVP;
	VkDescriptorSetLayout descriptorSetLayout_1UC_4SC;
	std::vector<VkDescriptorSetLayout> descriptorSetLayout_lbvh;

	std::vector<VkDescriptorSet> descriptorSets_multi_MPV_TS_TRN;
	std::vector<VkDescriptorSet> descriptorSets_uniformMVP;
	std::vector<VkDescriptorSet> descriptorSets_1UC_4SC;
	std::vector<VkDescriptorSet> descriptorSets_lbvh;

private:
	VkDevice* pDevice;

	void createDS_single(
		VkDescriptorSetLayout* pDescriptorSetLayout, std::vector<VkDescriptorSet>& pDescriptionSet, std::vector<VkBuffer>& pBuffer,
		unsigned long long range,
		int binding,
		VkDescriptorType descriptorType
	);
};
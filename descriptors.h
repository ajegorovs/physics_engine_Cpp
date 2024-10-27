#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>


class Descriptors {
public:
	Descriptors();
	Descriptors(VkDevice * pDevice);

	void createDescriptorPool();

	void createDSL_multi_MPV_TS_TRN(); // Swapchain->createDescriptorPool, createDS_multi_MPV_TS_TRN
	void createDSL_storageParticles();  // as vertex buffer
	void createDSL_uniformMVP();		// MVP only

	void createDS_single(
		VkDescriptorSetLayout * pDescriptorSetLayout, std::vector<VkDescriptorSet>& pDescriptionSet, std::vector<VkBuffer>& pBuffer,
		unsigned long long range,
		int binding,
		VkDescriptorType descriptorType
	);

	void createDS_multi_MPV_TS_TRN(std::vector<VkBuffer> uniformBuffers, std::vector<VkBuffer> storageBuffer, VkImageView textureImageView, VkSampler textureSampler);
	void createDS_uniformMVP(std::vector<VkBuffer>& pUniformBuffer);
	void createDS_storageParticles(std::vector<VkBuffer>& pStorageBuffer);

	void cleanupDSL();
	void cleanupDPool();

	VkDescriptorPool descriptorPool;

	VkDescriptorSetLayout descriptorSetLayout_multi_MPV_TS_TRN;
	VkDescriptorSetLayout descriptorSetLayout_uniformMVP;
	VkDescriptorSetLayout descriptorSetLayout_storageParticles;

	std::vector<VkDescriptorSet> descriptorSets_multi_MPV_TS_TRN;
	std::vector<VkDescriptorSet> descriptorSets_storageParticles;
	std::vector<VkDescriptorSet> descriptorSets_uniformMVP;

private:
	VkDevice* pDevice;
};
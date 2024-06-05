#pragma once

#include <vulkan/vulkan_core.h>
#include <expected.hpp>

#include "error.h"
#include "allocstructs.h"
#include "fence.h"
#include "deletionqueue.h"

struct FrameDeletion {
    delFunc destroySync;
    delFunc destroyDescriptors;
    delFunc destroyCommands;
};

struct Frame {
    VkSemaphore _presentSemaphore, _renderSemaphore;
	Fence _renderFence;

	DeletionQueue _frameDeletionQueue;

	VkCommandPool _commandPool;
	VkCommandBuffer _mainCommandBuffer;

	AllocatedBuffer cameraBuffer;
	VkDescriptorSet globalDescriptor;

	AllocatedBuffer objectBuffer;
	VkDescriptorSet objectDescriptor;

    tl::expected<FrameDeletion, VulkanError*> create(uint32_t queueFamilyIndex, VkDescriptorPool descriptorPool, VkDescriptorSetLayout globalLayout, VkDescriptorSetLayout objectLayout, AllocatedBuffer sceneBuffer);
    tl::expected<delFunc, VulkanError*> createSync();
    tl::expected<delFunc, VulkanError*> createDescriptors(VkDescriptorPool descriptorPool, VkDescriptorSetLayout globalLayout, VkDescriptorSetLayout objectLayout, AllocatedBuffer sceneBuffer);
    tl::expected<delFunc, VulkanError*> createCommands(uint32_t queueFamilyIndex);
};
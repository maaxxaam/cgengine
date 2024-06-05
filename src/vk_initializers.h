// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <vector>

namespace vkinit {
	namespace createinfo {
		VkCommandPoolCreateInfo commandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags = 0);

		VkFenceCreateInfo fence(VkFenceCreateFlags flags = 0);

		VkSemaphoreCreateInfo semaphore(VkSemaphoreCreateFlags flags = 0);

		VkImageCreateInfo image(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);

		VkImageViewCreateInfo imageView(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);

		VkSamplerCreateInfo sampler(VkFilter filters, VkSamplerAddressMode samplerAdressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT);

		VkFramebufferCreateInfo framebuffer(VkRenderPass renderPass, VkExtent2D extent);

		VkShaderModuleCreateInfo shaderModule(std::vector<uint32_t>& moduleBinary);
	}

	VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool pool, uint32_t count = 1, VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

	VkCommandBufferBeginInfo command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0);

	VkSubmitInfo submit_info(VkCommandBuffer* cmd);

	VkPresentInfoKHR present_info();

	VkRenderPassBeginInfo renderpass_begin_info(VkRenderPass renderPass, VkExtent2D windowExtent, VkFramebuffer framebuffer);

	VkDescriptorSetLayoutBinding descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding);

	VkWriteDescriptorSet write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo, uint32_t binding);

	VkWriteDescriptorSet write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding);
}


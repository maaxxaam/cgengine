#include "vk_initializers.h"
#include <vulkan/vulkan_core.h>

VkCommandPoolCreateInfo vkinit::createinfo::commandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags /*= 0*/) {
	return VkCommandPoolCreateInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = flags,
		.queueFamilyIndex = queueFamilyIndex
	};
}

VkCommandBufferAllocateInfo vkinit::command_buffer_allocate_info(VkCommandPool pool, uint32_t count /*= 1*/, VkCommandBufferLevel level /*= VK_COMMAND_BUFFER_LEVEL_PRIMARY*/) {
	return VkCommandBufferAllocateInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,

		.commandPool = pool,
		.level = level,
		.commandBufferCount = count
	};
}

VkCommandBufferBeginInfo vkinit::command_buffer_begin_info(VkCommandBufferUsageFlags flags /*= 0*/) {
	return VkCommandBufferBeginInfo {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = flags,
		.pInheritanceInfo = nullptr
	};
}

VkFramebufferCreateInfo vkinit::createinfo::framebuffer(VkRenderPass renderPass, VkExtent2D extent) {
	return VkFramebufferCreateInfo {
		.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
		.pNext = nullptr,

		.renderPass = renderPass,
		.attachmentCount = 1,
		.width = extent.width,
		.height = extent.height,
		.layers = 1
	};
}

VkFenceCreateInfo vkinit::createinfo::fence(VkFenceCreateFlags flags /*= 0*/) {
	return VkFenceCreateInfo {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.pNext = nullptr,
		.flags = flags
	};
}

VkSemaphoreCreateInfo vkinit::createinfo::semaphore(VkSemaphoreCreateFlags flags /*= 0*/) {
	return VkSemaphoreCreateInfo {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.pNext = nullptr,
		.flags = flags
	};
}

VkSubmitInfo vkinit::submit_info(VkCommandBuffer* cmd) {
	return VkSubmitInfo {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.pNext = nullptr,

		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.pWaitDstStageMask = nullptr,
		.commandBufferCount = 1,
		.pCommandBuffers = cmd,
		.signalSemaphoreCount = 0,
		.pSignalSemaphores = nullptr
	};
}

VkPresentInfoKHR vkinit::present_info() {
	return VkPresentInfoKHR {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
		.pNext = nullptr,

		.waitSemaphoreCount = 0,
		.pWaitSemaphores = nullptr,
		.swapchainCount = 0,
		.pSwapchains = nullptr,
		.pImageIndices = nullptr
	};
}

VkRenderPassBeginInfo vkinit::renderpass_begin_info(VkRenderPass renderPass, VkExtent2D windowExtent, VkFramebuffer framebuffer) {
	return VkRenderPassBeginInfo {
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,

		.renderPass = renderPass,
		.framebuffer = framebuffer,
		.renderArea {
			.offset = {
				.x = 0,
				.y = 0
			},
			.extent = windowExtent
		},
		.clearValueCount = 1,
		.pClearValues = nullptr
	};
}

VkShaderModuleCreateInfo vkinit::createinfo::shaderModule(std::vector<uint32_t>& moduleBinary) {
	return VkShaderModuleCreateInfo {
		.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
		.pNext = nullptr,
		// codeSize is in bytes, so multply buffer length by item size to know real size of the buffer
		.codeSize = moduleBinary.size() * sizeof(uint32_t),
		.pCode = moduleBinary.data()
	};
}

VkImageCreateInfo vkinit::createinfo::image(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent) {
	return VkImageCreateInfo {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.pNext = nullptr,

		.imageType = VK_IMAGE_TYPE_2D,
		.format = format,
		.extent = extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = usageFlags
	};
}

VkImageViewCreateInfo vkinit::createinfo::imageView(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags) {
	//build a image-view for the depth image to use for rendering
	return VkImageViewCreateInfo {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.pNext = nullptr,

		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.subresourceRange = {
			.aspectMask = aspectFlags,
			.baseMipLevel = 0,
			.levelCount = 1,
			.baseArrayLayer = 0,
			.layerCount = 1
		}
	};
}

VkDescriptorSetLayoutBinding vkinit::descriptorset_layout_binding(VkDescriptorType type, VkShaderStageFlags stageFlags, uint32_t binding) {
	return VkDescriptorSetLayoutBinding {
		.binding = binding,
		.descriptorType = type,
		.descriptorCount = 1,
		.stageFlags = stageFlags,
		.pImmutableSamplers = nullptr
	};
}

VkWriteDescriptorSet vkinit::write_descriptor_buffer(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorBufferInfo* bufferInfo , uint32_t binding) {
	return VkWriteDescriptorSet {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext = nullptr,

		.dstSet = dstSet,
		.dstBinding = binding,
		.descriptorCount = 1,
		.descriptorType = type,
		.pBufferInfo = bufferInfo
	};
}

VkWriteDescriptorSet vkinit::write_descriptor_image(VkDescriptorType type, VkDescriptorSet dstSet, VkDescriptorImageInfo* imageInfo, uint32_t binding) {
	return VkWriteDescriptorSet {
		.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		.pNext = nullptr,

		.dstSet = dstSet,
		.dstBinding = binding,
		.descriptorCount = 1,
		.descriptorType = type,
		.pImageInfo = imageInfo
	};
}

VkSamplerCreateInfo vkinit::createinfo::sampler(VkFilter filters, VkSamplerAddressMode samplerAdressMode /*= VK_SAMPLER_ADDRESS_MODE_REPEAT*/) {
	return VkSamplerCreateInfo {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.pNext = nullptr,
		
		.magFilter = filters,
		.minFilter = filters,
		.addressModeU = samplerAdressMode,
		.addressModeV = samplerAdressMode,
		.addressModeW = samplerAdressMode,
	};
}

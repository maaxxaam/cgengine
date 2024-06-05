#include "devicesingleton.h"
#include "expected.hpp"
#include "src/error.h"
#include <optional>
#include <vulkan/vulkan_core.h>
#include "vk_operations.h"

namespace vkcommand {

tl::expected<uint32_t, VulkanError*> acquireNextImage(VkSwapchainKHR swapchain, VkSemaphore semaphore) {
    uint32_t result;
    VkResult acquireResult = vkAcquireNextImageKHR(DeviceRef(), swapchain, UINT64_MAX, semaphore, nullptr, &result);

	switch (acquireResult) {
		case VK_SUCCESS: break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return tl::unexpected(new VulkanError(acquireResult, ErrorMessage("Out-of-memory (TODO: probably recoverable)")));
		case VK_ERROR_DEVICE_LOST:
		case VK_ERROR_SURFACE_LOST_KHR:
			return tl::unexpected(new VulkanError(acquireResult, ErrorMessage("Device or surface lost (TODO: should recreate object in this case)")));
		// TODO: handle window resize
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		case VK_SUBOPTIMAL_KHR:
			return tl::unexpected(new VulkanError(acquireResult, ErrorMessage("TODO: handle window resize")));
		// TODO: What do VK_TIMEOUT and VK_NOT_READY codes mean in this context? How would you handle those? 
		// Docs mention vkAcquireNextImageKHR may return them, but never mentions when or why
		case VK_TIMEOUT:
		case VK_NOT_READY:
		default:
			return tl::unexpected(new VulkanError(acquireResult, ErrorMessage("Unknown Vulkan error")));
	}

    return result;
}

tl::expected<std::vector<VkDescriptorSet>, VulkanError*> allocateDescriptorSets(const VkDescriptorSetAllocateInfo *pAllocateInfo) {
    std::vector<VkDescriptorSet> result;
    result.resize(pAllocateInfo->descriptorSetCount);
    VkResult operationResult = vkAllocateDescriptorSets(DeviceRef(), pAllocateInfo, result.data());

    VK_CHECK_OOM(operationResult);
    if (operationResult == VK_ERROR_OUT_OF_POOL_MEMORY) {
        return tl::unexpected(new VulkanError(operationResult, ErrorMessage("Could not allocate global descriptors: not enough memory in pool")));
    } else if (operationResult == VK_ERROR_FRAGMENTED_POOL) {
        return tl::unexpected(new VulkanError(operationResult, ErrorMessage("Could not allocate global descriptors due to pool fragmentation")));
    }

    return result;
}

tl::expected<VkDescriptorSet, VulkanError*> allocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout) {
    std::vector<VkDescriptorSet> result;
	VkDescriptorSetAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr,

		.descriptorPool = pool,
		.descriptorSetCount = 1,
		.pSetLayouts = &layout
	};
    result.resize(allocInfo.descriptorSetCount);
    VkResult operationResult = vkAllocateDescriptorSets(DeviceRef(), &allocInfo, result.data());

    VK_CHECK_OOM(operationResult);
    if (operationResult == VK_ERROR_OUT_OF_POOL_MEMORY) {
        return tl::unexpected(new VulkanError(operationResult, ErrorMessage("Could not allocate global descriptors: not enough memory in pool")));
    } else if (operationResult == VK_ERROR_FRAGMENTED_POOL) {
        return tl::unexpected(new VulkanError(operationResult, ErrorMessage("Could not allocate global descriptors due to pool fragmentation")));
    }

    return result[0];
}

std::optional<VulkanError*> beginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo& pBeginInfo) {
	VkResult beginCBufferResult = vkBeginCommandBuffer(commandBuffer, &pBeginInfo);

	switch (beginCBufferResult) {
		case VK_SUCCESS: return std::nullopt;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return new VulkanError(beginCBufferResult, ErrorMessage("Out-of-memory (TODO: probably recoverable)"));
		default:
			return new VulkanError(beginCBufferResult, ErrorMessage("Unknown Vulkan error"));
	}
}

std::optional<VulkanError*> endCommandBuffer(VkCommandBuffer commandBuffer) {
	VkResult endCBufferResult = vkEndCommandBuffer(commandBuffer);

	switch (endCBufferResult) {
		case VK_SUCCESS: return std::nullopt;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return new VulkanError(endCBufferResult, ErrorMessage("Out-of-memory (TODO: probably recoverable)"));
		case VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR:
			return new VulkanError(endCBufferResult, ErrorMessage("Video compression parameters error"));
		default:
			return new VulkanError(endCBufferResult, ErrorMessage("Unknown Vulkan error"));
	}
}

tl::expected<VkDescriptorSetLayout, VulkanError*> createDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo) {
	VkDescriptorSetLayout result;
	VkResult operationResult = vkCreateDescriptorSetLayout(DeviceRef(), pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(operationResult);
	return result;
}

tl::expected<VkDescriptorPool, VulkanError*> createDescriptorPool(const VkDescriptorPoolCreateInfo *pCreateInfo) {
	VkDescriptorPool result;
	VkResult operationResult = vkCreateDescriptorPool(DeviceRef(), pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(operationResult);
	if (operationResult == VK_ERROR_FRAGMENTATION_EXT) {
		return tl::unexpected(new VulkanError(operationResult, ErrorMessage("Cannot allocate descriptor pool due to fragmentation")));
	}
	return result;
}

std::optional<VulkanError*> singleQueueSubmit(VkQueue queue, VkSubmitInfo& submitInfo, VkFence& fence) {
	VkResult submitResult = vkQueueSubmit(queue, 1, &submitInfo, fence);

	switch (submitResult) {
		case VK_SUCCESS: return std::nullopt;
		case VK_ERROR_DEVICE_LOST:
			return new VulkanError(submitResult, ErrorMessage("Unrecoverable out-of-memory error (TODO: you can theoretically recreate VkDevice in this case)"));
		case VK_ERROR_OUT_OF_HOST_MEMORY:
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return new VulkanError(submitResult, ErrorMessage("Out-of-memory (TODO: you can theoretically recover from this)"));
		default:
			return new VulkanError(submitResult, ErrorMessage("Unknown Vulkan error"));
	}
}

std::optional<VulkanError*> waitForFence(const VkFence *pFence, const uint64_t timeout) {
	VkResult fenceResult = vkWaitForFences(DeviceRef(), 1, pFence, true, timeout);

	switch (fenceResult) {
		case VK_TIMEOUT:
		case VK_SUCCESS: return std::nullopt;
		case VK_ERROR_OUT_OF_HOST_MEMORY:
			return new VulkanError(fenceResult, ErrorMessage("Out-of-memory (TODO: can probably recover from that)"));
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return new VulkanError(fenceResult, ErrorMessage("Out-of-memory (TODO: can probably recover from that)"));
		case VK_ERROR_DEVICE_LOST:
			return new VulkanError(fenceResult, ErrorMessage("Device lost (TODO: should recreate VkDevice in this case)"));
		default:
			return new VulkanError(fenceResult, ErrorMessage("Unknown Vulkan error"));
	}
}

std::optional<VulkanError*> resetFence(const VkFence *pFence) {
	VkResult fenceResult = vkResetFences(DeviceRef(), 1, pFence);

	switch (fenceResult) {
		case VK_SUCCESS: return std::nullopt;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return new VulkanError(fenceResult, ErrorMessage("Out-of-memory (TODO: can probably recover from that)"));
		default:
			return new VulkanError(fenceResult, ErrorMessage("Unknown Vulkan error"));
	}
}

std::optional<VulkanError*> resetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
	VkResult poolResetResult = vkResetCommandPool(DeviceRef(), commandPool, flags);

	if (poolResetResult == VK_ERROR_OUT_OF_DEVICE_MEMORY) {
		return new VulkanError(poolResetResult, ErrorMessage("Out-of-memory (TODO: possible to recover)"));
	}
	return std::nullopt;
}

tl::expected<VkSampler, VulkanError*> createSampler(const VkSamplerCreateInfo& pCreateInfo) {
	VkSampler result;
	VkResult operationResult = vkCreateSampler(DeviceRef(), &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(operationResult);
	if (operationResult == VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR) {
		return tl::unexpected(new VulkanError(operationResult, ErrorMessage("Tried creating sampler on unavailable address")));
	}

	return result;
}

tl::expected<VkImageView, VulkanError*> createImageView(const VkImageViewCreateInfo& pCreateInfo) {
	VkImageView result;
	VkResult imageViewResult = vkCreateImageView(DeviceRef(), &pCreateInfo, nullptr, &result);

	VK_CHECK_OOM(imageViewResult);
	if (imageViewResult == VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS_KHR) {
		return tl::unexpected(new VulkanError(imageViewResult, ErrorMessage("Tried creating depth image view on unavailable address")));
	}

	return result;
}

tl::expected<VkPipeline, VulkanError*> createGraphicsPipeline(VkPipelineCache pipelineCache, const VkGraphicsPipelineCreateInfo& pCreateInfo) {
	VkPipeline result;
	VkResult pipeResult = vkCreateGraphicsPipelines(DeviceRef(), pipelineCache, 1, &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(pipeResult);
	if (pipeResult == VK_ERROR_INVALID_SHADER_NV) {
		return tl::unexpected(new VulkanError(pipeResult, ErrorMessage("Failed to create pipeline: invalid shaders")));
	}

	return result;
}

tl::expected<VkShaderModule, VulkanError*> createShaderModule(const VkShaderModuleCreateInfo& pCreateInfo) {
	VkShaderModule result;
	VkResult createResult = vkCreateShaderModule(DeviceRef(), &pCreateInfo, nullptr, &result); 

	VK_CHECK_OOM(createResult);
	if (createResult == VK_ERROR_INVALID_SHADER_NV) {
		return tl::unexpected(new VulkanError(createResult, ErrorMessage("Shader compile/link error. Enable VK_EXT_debug_report extension for more info")));
	}

	return result;
}

tl::expected<VkPipelineLayout, VulkanError*> createPipelineLayout(const VkPipelineLayoutCreateInfo& pCreateInfo) {
	VkPipelineLayout result;
	VkResult pipeLayoutResult = vkCreatePipelineLayout(DeviceRef(), &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(pipeLayoutResult);
	return result;
}

tl::expected<VkSemaphore, VulkanError*> createSemaphore(const VkSemaphoreCreateInfo& pCreateInfo) {
	VkSemaphore result;
	VkResult createSync = vkCreateSemaphore(DeviceRef(), &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(createSync);
	return result;
}

tl::expected<VkFence, VulkanError*> createFence(const VkFenceCreateInfo& pCreateInfo) {
	VkFence result;
	VkResult createSync = vkCreateFence(DeviceRef(), &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(createSync);
	return result;
}

tl::expected<VkCommandPool, VulkanError*> createCommandPool(const VkCommandPoolCreateInfo& pCreateInfo) {
	VkCommandPool result;
	VkResult poolResult = vkCreateCommandPool(DeviceRef(), &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(poolResult);
	return result;
}

tl::expected<VkCommandBuffer, VulkanError*> allocateCommandBuffer(const VkCommandBufferAllocateInfo& pAllocateInfo) {
	VkCommandBuffer result;
	VkResult allocResult = vkAllocateCommandBuffers(DeviceRef(), &pAllocateInfo, &result);
	VK_CHECK_OOM(allocResult);
	return result;
}

tl::expected<VkFramebuffer, VulkanError*> createFramebuffer(const VkFramebufferCreateInfo& pCreateInfo) {
	VkFramebuffer result;
	VkResult framebufferResult = vkCreateFramebuffer(DeviceRef(), &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(framebufferResult);
	return result;
}

tl::expected<VkRenderPass, VulkanError*> createRenderPass(const VkRenderPassCreateInfo& pCreateInfo) {
	VkRenderPass result;
	VkResult createRenderPassResult = vkCreateRenderPass(DeviceRef(), &pCreateInfo, nullptr, &result);
	VK_CHECK_OOM(createRenderPassResult);
	return result;
}

std::optional<VulkanError*> resetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags) {
	VkResult resetCBufferResult = vkResetCommandBuffer(commandBuffer, flags);

	switch (resetCBufferResult) {
		case VK_SUCCESS: break;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return new VulkanError(resetCBufferResult, ErrorMessage("Out-of-memory (TODO: can probably recover from that)"));
		default:
			return new VulkanError(resetCBufferResult, ErrorMessage("Unknown Vulkan error"));
	}

	return std::nullopt;
}

std::optional<VulkanError*> queuePresent(VkQueue queue, const VkPresentInfoKHR& pPresentInfo) {
	VkResult presentResult = vkQueuePresentKHR(queue, &pPresentInfo);

	switch (presentResult) {
		case VK_SUCCESS: break;
		case VK_ERROR_DEVICE_LOST:
		case VK_ERROR_SURFACE_LOST_KHR:
			return new VulkanError(presentResult, ErrorMessage("Device or surface unavailable"));
		// TODO: handle window resize
		case VK_ERROR_OUT_OF_DATE_KHR:
		case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		case VK_SUBOPTIMAL_KHR:
			return new VulkanError(presentResult, ErrorMessage("TODO: handle window resize"));
		default:
			return new VulkanError(presentResult, ErrorMessage("Unknown Vulkan error"));
	}

	return std::nullopt;
}

}; // End of namespace vkcommand

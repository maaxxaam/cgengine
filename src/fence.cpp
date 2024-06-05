#include "devicesingleton.h"
#include "fence.h"

std::optional<VulkanError*> Fence::wait(const uint64_t timeout) {
	VkResult fenceResult = vkWaitForFences(DeviceRef(), 1, &_fence, true, timeout);

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

std::optional<VulkanError*> Fence::reset() {
	VkResult fenceResult = vkResetFences(DeviceRef(), 1, &_fence);

	switch (fenceResult) {
		case VK_SUCCESS: return std::nullopt;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return new VulkanError(fenceResult, ErrorMessage("Out-of-memory (TODO: can probably recover from that)"));
		default:
			return new VulkanError(fenceResult, ErrorMessage("Unknown Vulkan error"));
	}
}

void Fence::destroy() {
    vkDestroyFence(DeviceRef(), _fence, nullptr);
}

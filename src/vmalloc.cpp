#include "vmalloc.h"
#include "expected.hpp"
#include "src/allocstructs.h"
#include "src/error.h"
#include <vulkan/vulkan_core.h>

MaybeVulkanError VMAllocator::create(VmaAllocatorCreateInfo& createInfo) {
    VkResult createResult = vmaCreateAllocator(&createInfo, &_allocator);

    if (createResult != VK_SUCCESS) {
        return new VulkanError(createResult, ErrorMessage("Failed to create VMA library allocator"));
    }

    return std::nullopt;
}

tl::expected<AllocatedBuffer, VulkanError*> VMAllocator::createBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage) {
    VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext = nullptr;
	bufferInfo.size = allocSize;

	bufferInfo.usage = usage;

	//let the VMA library know that this data should be writeable by CPU, but also readable by GPU
	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage = memoryUsage;

	AllocatedBuffer newBuffer;
    VkResult createResult = vmaCreateBuffer(_allocator, &bufferInfo, &vmaallocInfo,
		&newBuffer._buffer,
		&newBuffer._allocation,
		&newBuffer._allocInfo);

	if (createResult < 0) {
		return tl::unexpected(new VulkanError(createResult, 
		ErrorMessage("Failed to create buffer with size {}; usage flags {}, {}", allocSize, usage, (int)memoryUsage)));
	}

    return newBuffer;
}

void VMAllocator::destroy() {
    vmaDestroyAllocator(_allocator);
}

void VMAllocator::destroyBuffer(AllocatedBuffer& buffer) {
    vmaDestroyBuffer(_allocator, buffer._buffer, buffer._allocation);
}

void VMAllocator::unmapBuffer(AllocatedBuffer& buffer) {
   	vmaUnmapMemory(_allocator, buffer._allocation);
}

tl::expected<void*, VulkanError*> VMAllocator::mapBuffer(AllocatedBuffer& buffer) {
    void* result;
	VkResult operationResult = vmaMapMemory(_allocator, buffer._allocation, &result);

	if (operationResult < 0) {
		return tl::unexpected(new VulkanError(operationResult, "Failed to map buffer to a pointer"));
	}

    return result;
}

tl::expected<AllocatedImage, VulkanError*> VMAllocator::createImage(VkMemoryPropertyFlags flags, VmaMemoryUsage memoryUsage, VkImageCreateInfo& imgInfo) {
    AllocatedImage result;

	VmaAllocationCreateInfo dimg_allocinfo = {
	    .usage = memoryUsage,
	    .requiredFlags = flags,
    };

	//allocate and create the image
	VkResult depthImageResult = vmaCreateImage(_allocator, &imgInfo, &dimg_allocinfo, &result._image, &result._allocation, &result._allocInfo);

	if (depthImageResult < 0) { // VMA docs state that fail returns "negative error code" and nothing else...
		return tl::unexpected(new VulkanError(depthImageResult, ErrorMessage("Failed to create depth image")));
	}

    return result;
}

void VMAllocator::destroyImage(AllocatedImage image) {
	vmaDestroyImage(_allocator, image._image, image._allocation);
}

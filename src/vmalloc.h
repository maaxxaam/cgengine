#pragma once

#include "allocstructs.h"
#include "error.h"
#include "expected.hpp"
#include "singleton.h"

class VMAllocator: public Singleton<VMAllocator> {
public:
    MaybeVulkanError create(VmaAllocatorCreateInfo& createInfo);
    void destroy();

    tl::expected<AllocatedBuffer, VulkanError*> createBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
    tl::expected<AllocatedImage, VulkanError*> createImage(VkMemoryPropertyFlags flags, VmaMemoryUsage memoryUsage, VkImageCreateInfo& imgInfo);
    tl::expected<void*, VulkanError*> mapBuffer(AllocatedBuffer& buffer);

    void destroyBuffer(AllocatedBuffer& buffer);
    void destroyImage(AllocatedImage image);
    void unmapBuffer(AllocatedBuffer& buffer);
private:
    VmaAllocator _allocator;
};

#define VMAlloc VMAllocator::instance()

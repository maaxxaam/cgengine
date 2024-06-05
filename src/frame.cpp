#include "src/deletionqueue.h"
#include "src/error.h"
#include "vk_operations.h"
#include "vk_initializers.h"
#include "devicesingleton.h"
#include "gpustructs.h"
#include "vmalloc.h"
#include "frame.h"

tl::expected<FrameDeletion, VulkanError*> Frame::create(uint32_t queueFamilyIndex, VkDescriptorPool descriptorPool, VkDescriptorSetLayout globalLayout, VkDescriptorSetLayout objectLayout, AllocatedBuffer sceneBuffer) {
    auto syncResult = createSync();
    VK_UNEXPECTED_ERROR(syncResult, "Failed to create sync primitives for frame");
    auto descResult = createDescriptors(descriptorPool, globalLayout, objectLayout, sceneBuffer);
    VK_UNEXPECTED_ERROR(descResult, "Failed to create descriptor sets for frame");
    auto commResult = createCommands(queueFamilyIndex);
    VK_UNEXPECTED_ERROR(commResult, "Failed to create command pool and buffers for frame");

    return FrameDeletion{
        syncResult.value(),
        descResult.value(),
        commResult.value()
    };
}

tl::expected<delFunc, VulkanError*> Frame::createSync() {
    VkFenceCreateInfo fenceCreateInfo = vkinit::createinfo::fence(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::createinfo::semaphore();
	tl::expected<VkFence, VulkanError*> fenceResult = vkcommand::createFence(fenceCreateInfo);
    VK_UNEXPECTED_ERROR(fenceResult, "Failed to create fence for frame");
    _renderFence = {fenceResult.value()};
    
    auto semaphoreResult = vkcommand::createSemaphore(semaphoreCreateInfo);
    VK_UNEXPECTED_ERROR(semaphoreResult, "Failed to create present semaphore for frame");
    _presentSemaphore = semaphoreResult.value();

    semaphoreResult = vkcommand::createSemaphore(semaphoreCreateInfo);
    VK_UNEXPECTED_ERROR(semaphoreResult, "Failed to create render semaphore for frame");
    _renderSemaphore = semaphoreResult.value();

    //enqueue the destruction of semaphores
    return [=]() {
        _renderFence.destroy();
        vkDestroySemaphore(DeviceRef(), _presentSemaphore, nullptr);
        vkDestroySemaphore(DeviceRef(), _renderSemaphore, nullptr);
        };
}

tl::expected<delFunc, VulkanError*> Frame::createDescriptors(VkDescriptorPool descriptorPool, VkDescriptorSetLayout globalLayout, VkDescriptorSetLayout objectLayout, AllocatedBuffer sceneBuffer) {
    auto createResult = VMAlloc.createBuffer(sizeof(GPUCameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    VK_UNEXPECTED_ERROR(createResult, "Failed to create a camera buffer")
    cameraBuffer = createResult.value();

    const int MAX_OBJECTS = 10000;
    createResult = VMAlloc.createBuffer(sizeof(GPUObjectData) * MAX_OBJECTS, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    VK_UNEXPECTED_ERROR(createResult, "Failed to create a buffer for scene parameters")
    objectBuffer = createResult.value();

    VkDescriptorSetAllocateInfo allocInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &globalLayout
    };

    auto allocResult = vkcommand::allocateDescriptorSets(&allocInfo);
    VK_UNEXPECTED_ERROR(allocResult, "Could not allocate global descriptor");
    globalDescriptor = allocResult.value()[0];

    VkDescriptorSetAllocateInfo objectSetAlloc = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorPool = descriptorPool,
        .descriptorSetCount = 1,
        .pSetLayouts = &objectLayout
    };

    allocResult = vkcommand::allocateDescriptorSets(&objectSetAlloc);
    VK_UNEXPECTED_ERROR(allocResult, "Could not allocate object descriptor");
    objectDescriptor = allocResult.value()[0];

    VkDescriptorBufferInfo cameraInfo {
        .buffer = cameraBuffer._buffer,
        .offset = 0,
        .range = sizeof(GPUCameraData)
    };

    VkDescriptorBufferInfo sceneInfo {
        .buffer = sceneBuffer._buffer,
        .offset = 0,
        .range = sizeof(GPUSceneData)
    };

    VkDescriptorBufferInfo objectBufferInfo {
        .buffer = objectBuffer._buffer,
        .offset = 0,
        .range = sizeof(GPUObjectData) * MAX_OBJECTS
    };

    VkWriteDescriptorSet cameraWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, globalDescriptor,&cameraInfo,0);
    
    VkWriteDescriptorSet sceneWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, globalDescriptor, &sceneInfo, 1);

    VkWriteDescriptorSet objectWrite = vkinit::write_descriptor_buffer(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, objectDescriptor, &objectBufferInfo, 0);

    VkWriteDescriptorSet setWrites[] = { cameraWrite,sceneWrite,objectWrite };
    vkUpdateDescriptorSets(DeviceRef(), 3, setWrites, 0, nullptr);

    return [=](){
        cameraBuffer.destroy();
        objectBuffer.destroy();
		};
}

tl::expected<delFunc, VulkanError*> Frame::createCommands(uint32_t queueFamilyIndex) {
    VkCommandPoolCreateInfo commandPoolInfo = vkinit::createinfo::commandPool(queueFamilyIndex, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	tl::expected<VkCommandPool, VulkanError*> poolResult = vkcommand::createCommandPool(commandPoolInfo);
    VK_UNEXPECTED_ERROR(poolResult, "Failed to create command buffer for frame");
    _commandPool = poolResult.value();

    // Allocate the default command buffer that we will use for rendering
    VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::command_buffer_allocate_info(_commandPool, 1);

	tl::expected<VkCommandBuffer, VulkanError*> bufferResult = vkcommand::allocateCommandBuffer(cmdAllocInfo);
    VK_UNEXPECTED_ERROR(bufferResult, "Failed to allocate main command buffer for frame");
    _mainCommandBuffer = bufferResult.value();

    return [=]() {
        vkDestroyCommandPool(DeviceRef(), _commandPool, nullptr);
    };
}


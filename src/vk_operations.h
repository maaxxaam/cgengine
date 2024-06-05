#pragma once

#include <vulkan/vulkan_core.h>
#include <expected.hpp>

#include <optional>

#include "error.h"

namespace vkcommand {
    tl::expected<uint32_t, VulkanError*> acquireNextImage(VkSwapchainKHR swapchain, VkSemaphore semaphore);

    tl::expected<std::vector<VkDescriptorSet>, VulkanError*> allocateDescriptorSets(const VkDescriptorSetAllocateInfo *pAllocateInfo);

    tl::expected<VkDescriptorSet, VulkanError*> allocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout);

    std::optional<VulkanError*> beginCommandBuffer(VkCommandBuffer commandBuffer, const VkCommandBufferBeginInfo& pBeginInfo);

    std::optional<VulkanError*> endCommandBuffer(VkCommandBuffer commandBuffer);

    tl::expected<VkDescriptorSetLayout, VulkanError*> createDescriptorSetLayout(const VkDescriptorSetLayoutCreateInfo* pCreateInfo);

    tl::expected<VkDescriptorPool, VulkanError*> createDescriptorPool(const VkDescriptorPoolCreateInfo *pCreateInfo);

    std::optional<VulkanError*> singleQueueSubmit(VkQueue queue, VkSubmitInfo& submitInfo, VkFence& fence);

    std::optional<VulkanError*> waitForFence(const VkFence *pFence, const uint64_t timeout);

    std::optional<VulkanError*> resetFence(const VkFence *pFence);

    std::optional<VulkanError*> resetCommandPool(VkCommandPool commandPool, VkCommandPoolResetFlags flags);

    tl::expected<VkSampler, VulkanError*> createSampler(const VkSamplerCreateInfo& pCreateInfo);

    tl::expected<VkImageView, VulkanError*> createImageView(const VkImageViewCreateInfo& pCreateInfo);

    tl::expected<VkPipeline, VulkanError*> createGraphicsPipeline(VkPipelineCache pipelineCache, const VkGraphicsPipelineCreateInfo& pCreateInfo);

    tl::expected<VkShaderModule, VulkanError*> createShaderModule(const VkShaderModuleCreateInfo& pCreateInfo);

    tl::expected<VkPipelineLayout, VulkanError*> createPipelineLayout(const VkPipelineLayoutCreateInfo& pCreateInfo);

    tl::expected<VkSemaphore, VulkanError*> createSemaphore(const VkSemaphoreCreateInfo& pCreateInfo);

    tl::expected<VkFence, VulkanError*> createFence(const VkFenceCreateInfo& pCreateInfo);

    tl::expected<VkCommandPool, VulkanError*> createCommandPool(const VkCommandPoolCreateInfo& pCreateInfo);

    tl::expected<VkCommandBuffer, VulkanError*> allocateCommandBuffer(const VkCommandBufferAllocateInfo& pAllocateInfo);

    tl::expected<VkFramebuffer, VulkanError*> createFramebuffer(const VkFramebufferCreateInfo& pCreateInfo);

    tl::expected<VkRenderPass, VulkanError*> createRenderPass(const VkRenderPassCreateInfo& pCreateInfo);

    std::optional<VulkanError*> resetCommandBuffer(VkCommandBuffer commandBuffer, VkCommandBufferResetFlags flags);

    std::optional<VulkanError*> queuePresent(VkQueue queue, const VkPresentInfoKHR& pPresentInfo);
}

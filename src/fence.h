#pragma once

#include <vulkan/vulkan_core.h>

#include <optional>

#include "error.h"

struct Fence {
    void destroy();

    std::optional<VulkanError*> wait(const uint64_t timeout);

    std::optional<VulkanError*> reset();

    VkFence _fence;

    VkFence& operator()() { return _fence; };
};

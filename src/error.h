#pragma once

#include <vulkan/vulkan_core.h>
#include <fmt/format.h>
#include <expected.hpp>

#include <string>
#include <optional>
#include <vector>

class Error {
public:
    Error(const std::string errorMessage): _message(errorMessage) {};
    Error(Error *error, const std::string extraErrorMessage): _nextInChain(error), _message(extraErrorMessage) {};
    ~Error();
    
    const std::string what();
private:
    Error *const _nextInChain = nullptr;
    const std::string _message;
};

using MaybeError = std::optional<Error *>;
using MaybeMultipleErrors = std::optional<std::vector<Error *>>;

#define ErrorMessage(...) fmt::format("{}:{}| {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

class VulkanError: public Error {
public:
    VulkanError(const VkResult errorCode, const std::string errorMessage);
    VulkanError(const VkResult errorCode, Error *error, const std::string extraErrorMessage);

    const VkResult getCode() const { return _errorCode; };
    bool isResizeError() const;
private:
    const VkResult _errorCode;
};

/*
 * A very specific macro to check for OOM Vulkan errors in VkResult
 * and return an unexpected object to a heap-allocated VulkanError object. 
 */
#define VK_CHECK_OOM(VAR) if (VAR == VK_ERROR_OUT_OF_HOST_MEMORY || VAR == VK_ERROR_OUT_OF_DEVICE_MEMORY) \
    return tl::unexpected(new VulkanError(VAR, ErrorMessage("Out-of-memory (TODO: you can theoretically recover from this)")))

#define VK_UNEXPECTED_ERROR(VAR, ...) if (!VAR.has_value()) { \
    return tl::unexpected(new VulkanError(VAR.error()->getCode(), VAR.error(), ErrorMessage(__VA_ARGS__))); \
}

#define VK_UNEXPECTED_OPT_ERROR(VAR, ...) if (VAR.has_value()) { \
    return tl::unexpected(new VulkanError(VAR.value()->getCode(), VAR.value(), ErrorMessage(__VA_ARGS__))); \
}

#define VK_OPTIONAL_ERROR(VAR, ...) if (!VAR.has_value()) { \
    return new VulkanError(VAR.error()->getCode(), VAR.error(), ErrorMessage(__VA_ARGS__)); \
}

#define VK_OPTIONAL_OPT_ERROR(VAR, ...) if (VAR.has_value()) { \
    return new VulkanError(VAR.value()->getCode(), VAR.value(), ErrorMessage(__VA_ARGS__)); \
}

using MaybeVulkanError = std::optional<VulkanError*>;

class FileError: public Error {
public:
    FileError(const int errorCode, const std::string errorMessage);
    FileError(const int errorCode, Error *error, const std::string extraErrorMessage);
private:
    const unsigned int _errorCode;
};


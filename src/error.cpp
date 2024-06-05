#include "error.h"

Error::~Error() {
    if (_nextInChain != nullptr) delete _nextInChain;
}

const std::string Error::what() {
    std::string result = _message;
    Error *next = _nextInChain;
    while (next != nullptr) {
        result.append("\n");
        result.append(next->_message);
        next = next->_nextInChain;
    }
    return result;
}

VulkanError::VulkanError(const VkResult errorCode, const std::string errorMessage): 
    _errorCode(errorCode), Error(errorMessage + fmt::format(" (Vulkan error code {})", (int)errorCode)) {}

VulkanError::VulkanError(const VkResult errorCode, Error *error, const std::string extraErrorMessage): 
    _errorCode(errorCode), Error(error, extraErrorMessage + fmt::format(" (Vulkan error code {})", (int)errorCode)) {}

FileError::FileError(const int errorCode, const std::string errorMessage): 
    _errorCode(errorCode), Error(errorMessage + fmt::format(" (File error code {})", errorCode)) {}

FileError::FileError(const int errorCode, Error *error, const std::string extraErrorMessage): 
    _errorCode(errorCode), Error(error, extraErrorMessage + fmt::format(" (File error code {})", errorCode)) {}

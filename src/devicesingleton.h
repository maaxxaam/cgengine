#pragma once

#include <vulkan/vulkan_core.h>

#include "singleton.h"

class DeviceSingleton: public Singleton<DeviceSingleton> {
public:
    void setRef(VkDevice device) { _device = device; };
    const VkDevice getRef() const { return _device; };
private:
    VkDevice _device;
};

#define DeviceRef() DeviceSingleton::instance().getRef()

#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/instance.h"
#include "vk/core/physical_device.h"

namespace neko::vk
{
class Instance;
class PhysicalDevice;
class Surface;

class ILogicalDevice
{
public:
    virtual explicit operator const VkDevice &() const = 0;
    [[nodiscard]] virtual const VkDevice& GetLogicalDevice() const = 0;
};

class NullLogicalDevice : public ILogicalDevice
{
public:
    explicit operator const VkDevice&() const override
    {
        neko_assert(false, "Vulkan Logical Device is null!")
        return nullptr;
    }

    [[nodiscard]] const VkDevice& GetLogicalDevice() const override
    {
        neko_assert(false, "Vulkan Logical Device is null!")
        return nullptr;
    }
};

class LogicalDevice : public ILogicalDevice
{
public:
    LogicalDevice(const Instance& instance,
            const PhysicalDevice& physicalDevice,
            const Surface& surface);
    ~LogicalDevice();

    explicit operator const VkDevice &() const override { return device_; }
    [[nodiscard]] const VkDevice& GetLogicalDevice() const override { return device_; }
    
    [[nodiscard]] const VkQueue& GetGraphicsQueue() const { return graphicsQueue_; }
    [[nodiscard]] const VkQueue& GetPresentQueue() const { return presentQueue_; }
private:
    const Instance& instance_;
    const PhysicalDevice& gpu_;
    const Surface& surface_;

    VkDevice device_{};
    VkQueue graphicsQueue_{};
    VkQueue presentQueue_{};
};

using LogicalDeviceLocator = Locator<ILogicalDevice, NullLogicalDevice>;
}
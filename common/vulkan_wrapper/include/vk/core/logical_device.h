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
	virtual ~ILogicalDevice() = default;
	
	virtual explicit operator const VkDevice &() const = 0;
    [[nodiscard]] virtual const VkDevice& GetLogicalDevice() const = 0;
};

class NullLogicalDevice final : public ILogicalDevice
{
public:
    explicit operator const VkDevice &() const override { neko_assert(false, "Logical Device is Null!") }
    [[nodiscard]] const VkDevice& GetLogicalDevice() const override { neko_assert(false, "Logical Device is Null!") }
};

class LogicalDevice final : public ILogicalDevice
{
public:
    explicit LogicalDevice() = default;

    void Init();
    void Destroy() const;

    explicit operator const VkDevice &() const override { return device_; }
    [[nodiscard]] const VkDevice& GetLogicalDevice() const override { return device_; }
    
    [[nodiscard]] const VkQueue& GetGraphicsQueue() const { return graphicsQueue_; }
    [[nodiscard]] const VkQueue& GetPresentQueue() const { return presentQueue_; }

private:
    VkDevice device_{};
    VkQueue graphicsQueue_{};
    VkQueue presentQueue_{};
};

using LogicalDeviceLocator = Locator<ILogicalDevice, NullLogicalDevice>;
}
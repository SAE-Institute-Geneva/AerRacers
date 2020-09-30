#pragma once

#include "engine/globals.h"
#include "vk/vulkan_include.h"
#include "vk/vk_utilities.h"
#include "vk/core/surface.h"

namespace neko::vk
{
class Surface;

class IPhysicalDevice
{
public:
    virtual explicit operator const VkPhysicalDevice &() const = 0;
    [[nodiscard]] virtual const VkPhysicalDevice& GetPhysicalDevice() const = 0;

    [[nodiscard]] virtual const QueueFamilyIndices& GetQueueFamilyIndices() const = 0;
};

class NullPhysicalDevice : public IPhysicalDevice
{
public:
    explicit operator const VkPhysicalDevice&() const override
    {
        neko_assert(false, "Vulkan Physical Device is null!")
        return nullptr;
    }

    [[nodiscard]] const VkPhysicalDevice& GetPhysicalDevice() const override
    {
        neko_assert(false, "Vulkan Physical Device is null!")
        return nullptr;
    }

    [[nodiscard]] const QueueFamilyIndices& GetQueueFamilyIndices() const override
    {
        neko_assert(false, "Vulkan Physical Device is null!")
        return {};
    }
};

class PhysicalDevice : public IPhysicalDevice
{
public:
    explicit PhysicalDevice(const Instance& instance, const Surface& surface);

    explicit operator const VkPhysicalDevice &() const override { return gpu_; }
    [[nodiscard]] const VkPhysicalDevice& GetPhysicalDevice() const override { return gpu_; }

    [[nodiscard]] bool IsDeviceSuitable(const VkPhysicalDevice& gpu) const;

    [[nodiscard]] const QueueFamilyIndices& GetQueueFamilyIndices() const override { return queueFamilyIndices_; }

private:
    const Instance& instance_;
    const Surface& surface_;

    VkPhysicalDevice gpu_;
    QueueFamilyIndices queueFamilyIndices_;
};

using PhysicalDeviceLocator = Locator<IPhysicalDevice, NullPhysicalDevice>;
}
#pragma once

#include "engine/globals.h"
#include "vk/vulkan_include.h"
#include "vk/vk_utilities.h"
#include "vk/core/surface.h"

namespace neko::vk
{
class Surface;

class PhysicalDevice
{
public:
    explicit PhysicalDevice() = default;

    void Init(const Instance& instance, const Surface& surface);

    explicit operator const VkPhysicalDevice &() const { return gpu_; }
    [[nodiscard]] const VkPhysicalDevice& GetPhysicalDevice() const { return gpu_; }

    [[nodiscard]] const QueueFamilyIndices& GetQueueFamilyIndices() const { return queueFamilyIndices_; }

private:
    VkPhysicalDevice gpu_{};
    QueueFamilyIndices queueFamilyIndices_{};
};
}
#pragma once

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

    void Init();

    explicit operator const VkPhysicalDevice &() const { return gpu_; }
    [[nodiscard]] const VkPhysicalDevice& GetPhysicalDevice() const { return gpu_; }

    [[nodiscard]] const QueueFamilyIndices& GetQueueFamilyIndices() const { return queueFamilyIndices_; }

private:
    VkPhysicalDevice gpu_{};
    QueueFamilyIndices queueFamilyIndices_{};
};

[[nodiscard]] static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface);

static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& gpu);
[[nodiscard]] static bool IsDeviceSuitable(
	const VkPhysicalDevice& gpu,
	const VkSurfaceKHR& surface,
	const QueueFamilyIndices& queueFamilyIndices);

static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface);
}
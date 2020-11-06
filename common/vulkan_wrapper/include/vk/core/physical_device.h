#pragma once
#include "vk/vulkan_include.h"
#include "vk/vk_utilities.h"

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

    [[nodiscard]] VkSampleCountFlagBits GetMsaaSamples() const;

    [[nodiscard]] static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface);

    static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& gpu);
    [[nodiscard]] static bool IsDeviceSuitable(
            const VkPhysicalDevice& gpu,
            const VkSurfaceKHR& surface,
            const QueueFamilyIndices& queueFamilyIndices);

    static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface);

private:
    VkPhysicalDevice gpu_{};
    QueueFamilyIndices queueFamilyIndices_{};
};
}
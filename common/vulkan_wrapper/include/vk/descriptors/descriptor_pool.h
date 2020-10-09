#pragma once
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
class DescriptorPool
{
public:
    explicit DescriptorPool() = default;

    void Init(const LogicalDevice& device, const Swapchain& swapchain);
    void Destroy(const LogicalDevice& device) const;

    explicit operator const VkDescriptorPool &() const { return descriptorPool_; }
    [[nodiscard]] const VkDescriptorPool& GetDescriptorPool() const { return descriptorPool_; }

private:
    VkDescriptorPool descriptorPool_{};
};
}
#pragma once
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
class DescriptorPool
{
public:
    explicit DescriptorPool(const LogicalDevice& device);

    void Init(const Swapchain& swapchain);
    void Destroy();

    explicit operator const VkDescriptorPool &() const { return descriptorPool_; }
    [[nodiscard]] const VkDescriptorPool& GetDescriptorPool() const { return descriptorPool_; }

private:
    const LogicalDevice& device_;

    VkDescriptorPool descriptorPool_{};
};
}
#pragma once
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
class DescriptorPool
{
public:
    explicit DescriptorPool() = default;

    void Init();
    void Destroy() const;

    explicit operator const VkDescriptorPool &() const { return descriptorPool_; }
    [[nodiscard]] const VkDescriptorPool& GetDescriptorPool() const { return descriptorPool_; }

private:
    VkDescriptorPool descriptorPool_{};
};
}
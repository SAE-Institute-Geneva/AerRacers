#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
class RenderPass
{
public:
    explicit RenderPass() = default;

    void Init(const PhysicalDevice& gpu, const LogicalDevice& device, const Swapchain& swapchain);
    void Destroy(const LogicalDevice& device) const;

    explicit operator const VkRenderPass &() const { return renderPass_; }
    [[nodiscard]] const VkRenderPass& GetRenderPass() const { return renderPass_; }

private:
    VkRenderPass renderPass_ = nullptr;
};
}
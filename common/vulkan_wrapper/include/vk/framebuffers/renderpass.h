#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
class RenderPass
{
public:
    RenderPass(const PhysicalDevice& gpu, const LogicalDevice& device, const Swapchain& swapchain);
    ~RenderPass();

    explicit operator const VkRenderPass &() const { return renderPass_; }
    [[nodiscard]] const VkRenderPass& GetRenderPass() const { return renderPass_; }

private:
    const PhysicalDevice& gpu_;
    const LogicalDevice& device_;
    const Swapchain& swapchain_;

    VkRenderPass renderPass_ = nullptr;
};
}
#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
class RenderPass
{
public:
    explicit RenderPass(const LogicalDevice& device);

    void Init(const PhysicalDevice& gpu, const Swapchain& swapchain);
    void Destroy();

    explicit operator const VkRenderPass &() const { return renderPass_; }
    [[nodiscard]] const VkRenderPass& GetRenderPass() const { return renderPass_; }

private:
    const LogicalDevice& device_;

    VkRenderPass renderPass_ = nullptr;
};
}
#pragma once
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/framebuffers/renderpass.h"
#include "vk/descriptors/descriptor_set.h"

namespace neko::vk
{
const std::array<VkDynamicState, 3> kDynamicStates =
{
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_LINE_WIDTH
};

class GraphicsPipeline
{
public:
    explicit GraphicsPipeline(
            const LogicalDevice& device,
            const Swapchain& swapchain,
            const RenderPass& renderPass);
    ~GraphicsPipeline();

    explicit operator const VkPipeline &() const { return pipeline_; }
    [[nodiscard]] const VkPipeline& GetGraphicsPipeline() const { return pipeline_; }

private:
    void CreateDescriptorSetLayout();

    const LogicalDevice& device_;
    const Swapchain& swapchain_;
    const RenderPass& renderPass_;

    VkPipeline pipeline_{};
    VkPipelineLayout layout_{};
    VkDescriptorSetLayout descriptorSetLayout_{};
};
}
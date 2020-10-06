#pragma once
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/framebuffers/renderpass.h"
#include "vk/descriptors/descriptor_set.h"

namespace neko::vk
{
const std::array<VkDynamicState, 2> kDynamicStates =
{
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    //VK_DYNAMIC_STATE_LINE_WIDTH
};

class GraphicsPipeline
{
public:
    explicit GraphicsPipeline(const LogicalDevice& device);

    void Init(const Swapchain& swapchain, const RenderPass& renderPass, DescriptorSets& descriptorSets);
    void Destroy();

    explicit operator const VkPipeline &() const { return pipeline_; }
    [[nodiscard]] const VkPipeline& GetGraphicsPipeline() const { return pipeline_; }
    [[nodiscard]] const VkPipelineLayout& GetLayout() const { return layout_; }

private:
    const LogicalDevice& device_;

    VkPipeline pipeline_{};
    VkPipelineLayout layout_{};
};
}
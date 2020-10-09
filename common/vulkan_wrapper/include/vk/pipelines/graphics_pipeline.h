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
    explicit GraphicsPipeline() = default;

    void Init(
        const LogicalDevice& device, 
        const Swapchain& swapchain, 
        const RenderPass& renderPass, 
        DescriptorSets& descriptorSets);
    void Destroy(const LogicalDevice& device) const;

    explicit operator const VkPipeline &() const { return pipeline_; }
    [[nodiscard]] const VkPipeline& GetGraphicsPipeline() const { return pipeline_; }
    [[nodiscard]] const VkPipelineLayout& GetLayout() const { return layout_; }

private:
    VkPipeline pipeline_{};
    VkPipelineLayout layout_{};
};

[[nodiscard]] static VkShaderModule CreateShaderModule(const VkDevice& device, const std::string_view& data);
[[nodiscard]] static VkVertexInputBindingDescription GetBindingDescription();
[[nodiscard]] static std::array<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions();
}
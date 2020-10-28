#pragma once
#include <utility>

#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/render_stage/render_stage.h"

namespace neko::vk
{
class RenderStage;

class RenderPass
{
public:
    class SubpassDescription
    {
    public:
        SubpassDescription(
                const VkPipelineBindPoint bindPoint,
                std::vector<VkAttachmentReference> colorAttachments,
                uint32_t depthAttachment = INVALID_INDEX)
                : subpassDescription_({})
                , colorAttachments_(std::move(colorAttachments))
                , depthStencilAttachment_({})
        {
            subpassDescription_.pipelineBindPoint = bindPoint;
            subpassDescription_.colorAttachmentCount = static_cast<uint32_t>(
                    colorAttachments_.size());
            subpassDescription_.pColorAttachments = colorAttachments_.data();

            if (depthAttachment != INVALID_INDEX)
            {
                depthStencilAttachment_.attachment = depthAttachment;
                depthStencilAttachment_.layout =
                        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                subpassDescription_.pDepthStencilAttachment = &
                        depthStencilAttachment_;
            }
        }

        explicit operator const VkSubpassDescription &() const { return subpassDescription_; }
        [[nodiscard]] const VkSubpassDescription& GetSubpassDescription() const
        { return subpassDescription_; }

    private:
        VkSubpassDescription subpassDescription_;
        std::vector<VkAttachmentReference> colorAttachments_;
        VkAttachmentReference depthStencilAttachment_;
    };

    void Init(const RenderStage& renderStage,
              VkFormat depthFormat,
              VkFormat surfaceFormat,
              VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
    void Destroy() const;

    explicit operator const VkRenderPass &() const { return renderPass_; }
    [[nodiscard]] const VkRenderPass& GetRenderPass() const { return renderPass_; }

private:
    VkRenderPass renderPass_ = nullptr;
};
}
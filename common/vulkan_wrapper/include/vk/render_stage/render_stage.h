#pragma once
#include <xxh3.h>

#include "vk/images/image_depth.h"
#include "vk/framebuffers/framebuffers.h"
#include "vk/framebuffers/renderpass.h"
#include "vk/render_stage/attachment.h"
#include "vk/render_stage/subpass_type.h"
#include "vk/render_stage/viewport.h"

namespace neko::vk
{
class Framebuffers;
class RenderPass;

class RenderStage
{
public:
    explicit RenderStage(std::vector<Attachment> renderPassAttachment,
                         std::vector<SubpassType> renderPassSubpasses);

    void Init(const std::vector<Attachment>& attachments = {},
            const std::vector<SubpassType>& subpasses = {},
            const Viewport& viewport = Viewport());

    void Update();
    void Rebuild(const Swapchain& swapchain);

    [[nodiscard]] const Attachment& GetAttachment(const std::string_view& name) const;
    [[nodiscard]] const Attachment& GetAttachment(uint32_t binding) const;
    [[nodiscard]] const VkFramebuffer& GetActiveFramebuffer(const uint32_t& activeSwapchainImage) const;
    [[nodiscard]] const std::vector<Attachment>& GetAttachments() const { return attachments_; }
    [[nodiscard]] const std::vector<SubpassType>& GetSubpasses() const { return subpasses_; }
    [[nodiscard]] const RenderPass* GetRenderPass() const { return renderPass_.get(); }
    [[nodiscard]] const Vec2u& GetSize() const { return size_; }
    [[nodiscard]] const std::vector<VkClearValue>& GetClearValues() const { return clearValues_; }
    [[nodiscard]] const std::map<XXH64_hash_t, const IDescriptor&>&
            GetDescriptors() const { return descriptors_; }

    [[nodiscard]] bool HasSwapchain() const;
    [[nodiscard]] bool IsOutOfDate() const { return outOfDate_; }

private:
    std::vector<Attachment> attachments_;
    std::vector<SubpassType> subpasses_;

    Viewport viewport_;

    std::unique_ptr<RenderPass> renderPass_;
    std::unique_ptr<ImageDepth> depthStencil_;
    std::unique_ptr<Framebuffers> framebuffers_;

    std::map<XXH64_hash_t, const IDescriptor&> descriptors_;

    std::vector<VkClearValue> clearValues_;
    std::vector<uint32_t> subpassAttachmentCount_;
    Attachment depthAttachment_{};
    Attachment swapchainAttachment_{};

    Vec2u size_{};
    float aspectRatio_ = 0.0f;
    bool outOfDate_{};
};
}
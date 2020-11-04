#include "vk/render_stage/render_stage.h"

#include <utility>

#include "mathematics/hash.h"
#include "vk/graphics.h"

namespace neko::vk
{
RenderStage::RenderStage(
        std::vector<Attachment> attachments,
        std::vector<SubpassType> subpasses,
        const Viewport& viewport)
        : attachments_(std::move(attachments)),
          subpasses_(std::move(subpasses)),
          viewport_(viewport),
          subpassAttachmentCount_(subpasses_.size())
{
    for (const auto& attachment : attachments_)
    {
        VkClearValue clearValue = {};

        switch (attachment.type)
        {
            case Attachment::Type::IMAGE:
                clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

                for (auto& subpass : subpasses_)
                {
                    const auto it = std::find(
                            subpass.attachmentBindings.begin(),
                            subpass.attachmentBindings.end(),
                            attachment.binding);
                    if (it != subpass.attachmentBindings.end())
                    {
                        subpassAttachmentCount_[subpass.binding]++;

                        if (attachment.multisampling)
                            subpass.multisampling = true;
                    }
                }
                break;
            case Attachment::Type::DEPTH:
                clearValue.depthStencil = {1.0f, 0};
                depthAttachment_ = Attachment(attachment);
                break;
            case Attachment::Type::SWAPCHAIN:
                clearValue.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
                swapchainAttachment_ = Attachment(attachment);
                break;
            default: ;
        }

        clearValues_.emplace_back(clearValue);
    }
}

void RenderStage::Update()
{
    const auto lastSize = size_;

    if (viewport_.size != Vec2u::zero) size_ = viewport_.size;
    else size_ = BasicEngine::GetInstance()->config.windowSize;

    size_ = Vec2u(size_.x * viewport_.scale.x, size_.y * viewport_.scale.y);
    aspectRatio_ = static_cast<float>(size_.x) / static_cast<float>(size_.y);

    outOfDate_ = size_ != lastSize;
}

void RenderStage::Rebuild(const Swapchain& swapchain)
{
    Update();

    const auto& vkObj = VkObjectsLocator::get();
    const auto msaaSamples = vkObj.gpu.GetMsaaSamples();

    if (depthAttachment_.type != Attachment::Type::NONE)
        depthStencil_ = std::make_unique<ImageDepth>(size_,
                              depthAttachment_.multisampling ? msaaSamples : VK_SAMPLE_COUNT_1_BIT);

    if (!renderPass_)
        renderPass_ = std::make_unique<RenderPass>(*this, depthStencil_->GetFormat(),
                vkObj.surface.GetFormat().format, msaaSamples);

    framebuffers_ = std::make_unique<Framebuffers>(size_.x, size_.y, *this, *renderPass_,
                        swapchain, *depthStencil_, msaaSamples);
    outOfDate_ = false;

    descriptors_.clear();

    for (const auto& attachment : attachments_)
    {
        const auto& nameHash = HashString(attachment.name);
        if (attachment.type == Attachment::Type::DEPTH)
            descriptors_.emplace(nameHash, *depthStencil_);
        else if (attachment.type != Attachment::Type::SWAPCHAIN)
            descriptors_.emplace(nameHash,
                    framebuffers_->GetAttachment(attachment.binding));
    }
}

const Attachment& RenderStage::GetAttachment(const std::string_view& name) const
{
    for (const auto& attachment : attachments_)
        if (attachment.name == name) return attachment;

    neko_assert(false, "Attachment with name " << name << " doesn't exist")
}

const Attachment& RenderStage::GetAttachment(uint32_t binding) const
{
    for (const auto& attachment : attachments_)
        if (attachment.binding == binding)
            return attachment;

    neko_assert(false, "Attachment with binding" << binding << " doesn't exist")
}

const VkFramebuffer& RenderStage::GetActiveFramebuffer(const uint32_t& activeSwapchainImage) const
{
    if (activeSwapchainImage > framebuffers_->GetFramebuffers().size())
        return framebuffers_->GetFramebuffers().at(0);

    return framebuffers_->GetFramebuffers().at(activeSwapchainImage);
}

bool RenderStage::HasSwapchain() const
{
    return swapchainAttachment_.type != Attachment::Type::NONE;
}

void RenderStage::Destroy() const
{
    depthStencil_->Destroy();
    framebuffers_->Destroy();
    renderPass_->Destroy();
}
}
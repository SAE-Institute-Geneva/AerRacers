#pragma once
#include "vk/framebuffers/swapchain.h"
#include "vk/framebuffers/renderpass.h"
#include "vk/images/image2d.h"
#include "vk/images/image_depth.h"
#include "vk/render_stage/render_stage.h"

namespace neko::vk
{
class RenderStage;
class RenderPass;

class Framebuffers
{
public:
    Framebuffers(
            uint32_t width,
            uint32_t height,
            const RenderStage& renderStage,
            const RenderPass& renderPass,
            const Swapchain& swapchain,
            const ImageDepth& depthStencil,
            VkSampleCountFlagBits samples);
	void Destroy() const;
	
    [[nodiscard]] const std::vector<VkFramebuffer>& GetFramebuffers() const {return framebuffers_;}
    [[nodiscard]] size_t GetFramebuffersCount() const {return framebuffers_.size();}

    const VkFramebuffer& operator[](const size_t index) const
    { return framebuffers_[index]; }

    VkFramebuffer& operator[](const size_t index)
    { return framebuffers_[index]; }

    [[nodiscard]] const Image2d& GetAttachment(uint32_t index) const
    { return *imageAttachments_[index]; }
	
private:
    std::vector<std::unique_ptr<Image2d>> imageAttachments_;
	std::vector<VkFramebuffer> framebuffers_;
};
}
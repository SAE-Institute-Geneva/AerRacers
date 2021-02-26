#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/framebuffers/framebuffers.h"
#include "vk/render_stage/attachment.h"
#include "vk/render_stage/subpass_type.h"
#include "vk/render_stage/viewport.h"

namespace neko::vk
{
class RenderStage
{
public:
	RenderStage(std::vector<Attachment> attachments = {},
		std::vector<SubpassType> subpasses          = {},
		const Viewport& viewport                    = {});

	void Update();
	void Rebuild(const Swapchain& swapchain);
	void Destroy() const;

	[[nodiscard]] const Attachment& GetAttachment(std::string_view name) const;
	[[nodiscard]] const Attachment& GetAttachment(std::uint32_t binding) const;
	[[nodiscard]] std::uint32_t GetAttachmentCount(std::uint32_t subpass) const;
	[[nodiscard]] const VkFramebuffer& GetActiveFramebuffer(uint32_t activeSwapchainImage) const;
	[[nodiscard]] const std::vector<Attachment>& GetAttachments() const { return attachments_; }
	[[nodiscard]] const std::vector<SubpassType>& GetSubpasses() const { return subpasses_; }
	[[nodiscard]] const RenderPass& GetRenderPass() { return *renderPass_; }
	[[nodiscard]] const Vec2u& GetSize() const { return size_; }
	[[nodiscard]] const std::vector<VkClearValue>& GetClearValues() const { return clearValues_; }
	[[nodiscard]] const std::map<StringHash, const IDescriptor&>& GetDescriptors() const
	{
		return descriptors_;
	}

	[[nodiscard]] bool HasSwapchain() const;
	[[nodiscard]] bool IsOutOfDate() const { return outOfDate_; }

private:
	std::vector<Attachment> attachments_ {};
	std::vector<SubpassType> subpasses_ {};

	Viewport viewport_ {};

	std::unique_ptr<RenderPass> renderPass_ {};
	std::unique_ptr<ImageDepth> depthStencil_ {};
	std::unique_ptr<Framebuffers> framebuffers_ {};

	std::map<StringHash, const IDescriptor&> descriptors_ {};

	std::vector<VkClearValue> clearValues_ {};
	std::vector<std::uint32_t> subpassAttachmentCount_ {};
	Attachment depthAttachment_ {};
	Attachment swapchainAttachment_ {};

	Vec2u size_     = Vec2u::zero;
	bool outOfDate_ = false;
};
}    // namespace neko::vk

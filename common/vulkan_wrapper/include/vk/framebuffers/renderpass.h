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
#include "vk/vk_include.h"

namespace neko::vk
{
class RenderStage;

class RenderPass
{
public:
	class SubpassDescription
	{
	public:
		SubpassDescription(const VkPipelineBindPoint bindPoint,
			std::vector<VkAttachmentReference> colorAttachments,
			const std::uint32_t depthAttachment = INVALID_INDEX)
		   : subpassDescription_({}),
			 colorAttachments_(std::move(colorAttachments)),
			 depthStencilAttachment_({})
		{
			subpassDescription_.pipelineBindPoint = bindPoint;
			subpassDescription_.colorAttachmentCount =
				static_cast<std::uint32_t>(colorAttachments_.size());
			subpassDescription_.pColorAttachments = colorAttachments_.data();

			if (depthAttachment != INVALID_INDEX)
			{
				depthStencilAttachment_.attachment = depthAttachment;
				depthStencilAttachment_.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
				subpassDescription_.pDepthStencilAttachment = &depthStencilAttachment_;
			}
		}

		operator const VkSubpassDescription&() const { return subpassDescription_; }

	private:
		VkSubpassDescription subpassDescription_;

		std::vector<VkAttachmentReference> colorAttachments_;
		VkAttachmentReference depthStencilAttachment_;
	};

	RenderPass(const RenderStage& renderStage,
		VkFormat depthFormat,
		VkFormat surfaceFormat,
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);
	void Destroy() const;

	operator const VkRenderPass&() const { return renderPass_; }

private:
	VkRenderPass renderPass_ {};
};
}    // namespace neko::vk
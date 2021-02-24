#include "vk/framebuffers/framebuffers.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
Framebuffers::Framebuffers(const std::uint32_t width,
	const std::uint32_t height,
	const RenderStage& renderStage,
	const RenderPass& renderPass,
	const Swapchain& swapchain,
	const ImageDepth& depthStencil,
	const VkSampleCountFlagBits samples)
{
	const VkResources* vkObj = VkResources::Inst;

	if (!imageAttachments_.empty())
	{
		imageAttachments_.clear();
		Destroy();
		framebuffers_.clear();
	}

	int index = 0;
	for (const auto& attachment : renderStage.GetAttachments())
	{
		auto attachmentSamples = attachment.multisampling ? samples : VK_SAMPLE_COUNT_1_BIT;

		switch (attachment.type)
		{
			case Attachment::Type::IMAGE:
				imageAttachments_.emplace_back(std::make_unique<Image2d>(Vec2u(width, height),
					attachment.format,
					VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
					VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
					VK_FILTER_LINEAR,
					VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
					attachmentSamples));
				break;
			default: imageAttachments_.emplace_back(nullptr); break;
		}

		index++;
	}

	const auto& imageViews      = swapchain.GetImageViews();
	const auto& swapchainExtent = swapchain.GetExtent();
	framebuffers_.resize(imageViews.size());

	for (std::size_t i = 0; i < imageViews.size(); i++)
	{
		std::vector<VkImageView> attachments;
		attachments.reserve(renderStage.GetAttachments().size());

		for (const auto& attachment : renderStage.GetAttachments())
		{
			switch (attachment.type)
			{
				case Attachment::Type::IMAGE:
					attachments.emplace_back(imageAttachments_.at(attachment.binding)->GetView());
					break;
				case Attachment::Type::DEPTH:
					attachments.emplace_back(depthStencil.GetView());
					break;
				case Attachment::Type::SWAPCHAIN: attachments.emplace_back(imageViews.at(i)); break;
				case Attachment::Type::NONE: break;
				default:;
			}
		}

		VkFramebufferCreateInfo framebufferInfo {};
		framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass      = VkRenderPass(renderPass);
		framebufferInfo.attachmentCount = static_cast<std::uint32_t>(attachments.size());
		framebufferInfo.pAttachments    = attachments.data();
		framebufferInfo.width           = swapchainExtent.width;
		framebufferInfo.height          = swapchainExtent.height;
		framebufferInfo.layers          = 1;

		const VkResult res =
			vkCreateFramebuffer(vkObj->device, &framebufferInfo, nullptr, &framebuffers_[i]);
		neko_assert(res == VK_SUCCESS, "Failed to create framebuffer!")
	}
}

void Framebuffers::Destroy() const
{
	for (const auto& attachment : imageAttachments_)
		if (attachment) attachment->Destroy();
	for (const auto& framebuffer : framebuffers_)
		vkDestroyFramebuffer(VkResources::Inst->device, framebuffer, nullptr);
}
}    // namespace neko::vk

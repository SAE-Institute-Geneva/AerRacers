#include "vk/framebuffers/framebuffers.h"

namespace neko::vk
{
Framebuffers::Framebuffers(const LogicalDevice& device, const Swapchain& swapChain, const RenderPass& renderPass)
	: device_(device), swapChain_(swapChain), renderPass_(renderPass)
{
	const auto& imageViews = swapChain.GetImageViews();
	const auto& swapchainExtent = swapChain.GetExtent();
	
    framebuffers_.resize(imageViews.size());

	for (size_t i = 0; i < imageViews.size(); i++) 
	{
		std::array<VkImageView, 2> attachments = {
		    imageViews[i],
		    imageViews[i],
		    //depthImage_.view
		};
	
	    VkFramebufferCreateInfo framebufferInfo{};
	    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	    framebufferInfo.renderPass = VkRenderPass(renderPass_);
	    framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	    framebufferInfo.pAttachments = attachments.data();
	    framebufferInfo.width = swapchainExtent.width;
	    framebufferInfo.height = swapchainExtent.height;
	    framebufferInfo.layers = 1;

		const VkResult res = 
			vkCreateFramebuffer(VkDevice(device_), &framebufferInfo, nullptr, &framebuffers_[i]);
		neko_assert(res == VK_SUCCESS, "Failed to create framebuffer!")
	}
}

Framebuffers::~Framebuffers()
{
	for (const auto& framebuffer : framebuffers_)
        vkDestroyFramebuffer(VkDevice(device_), framebuffer, nullptr);
}
}

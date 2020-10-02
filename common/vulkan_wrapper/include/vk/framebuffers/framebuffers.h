#pragma once
#include "vk/vulkan_include.h"
#include "vk/core/logical_device.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/framebuffers/renderpass.h"

namespace neko::vk
{
class Framebuffers
{
public:
	Framebuffers(const LogicalDevice& device, const Swapchain& swapChain, const RenderPass& renderPass);
	~Framebuffers();
	
    const std::vector<VkFramebuffer>& GetFramebuffers() const {return framebuffers_;}
    size_t GetFramebuffersCount() const {return framebuffers_.size();}
	
private:
	const LogicalDevice& device_;
	const Swapchain& swapChain_;
	const RenderPass& renderPass_;
	
	std::vector<VkFramebuffer> framebuffers_;
};
}
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
	explicit Framebuffers() = default;

	void Init(const LogicalDevice& device, const Swapchain& swapChain, const RenderPass& renderPass);
	void Destroy(const LogicalDevice& device);
	
    [[nodiscard]] const std::vector<VkFramebuffer>& GetFramebuffers() const {return framebuffers_;}
    [[nodiscard]] size_t GetFramebuffersCount() const {return framebuffers_.size();}

    const VkFramebuffer& operator[](const size_t index) const
    { return framebuffers_[index]; }

    VkFramebuffer& operator[](const size_t index)
    { return framebuffers_[index]; }
	
private:
	std::vector<VkFramebuffer> framebuffers_;
};
}
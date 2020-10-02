#pragma once
#include "vk/vulkan_include.h"
#include "vk/commands/command_pool.h"
#include "vk/framebuffers/framebuffers.h"
#include "vk/pipelines/graphics_pipeline.h"

namespace neko::vk
{
class CommandBuffers
{
public:
	CommandBuffers(
		const LogicalDevice& device, 
		const Swapchain& swapchain, 
		const RenderPass& renderPass,
		const GraphicsPipeline& graphicsPipeline,
		const CommandPool& commandPool, 
		const Framebuffers& framebuffers);

	const VkCommandBuffer& operator[](const size_t index) const
    { return commandBuffers_[index]; }

    VkCommandBuffer& operator[](const size_t index)
    { return commandBuffers_[index]; }
	
private:
	const LogicalDevice& device_;
	const Swapchain& swapchain_;
	const RenderPass& renderPass_;
	const GraphicsPipeline& graphicsPipeline_;
	const CommandPool& commandPool_;
	const Framebuffers& framebuffers_;
	
	std::vector<VkCommandBuffer> commandBuffers_;
};
}

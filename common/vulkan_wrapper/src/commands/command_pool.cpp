#include "vk/vk_resources.h"

namespace neko::vk
{
CommandPool::CommandPool() { Init(); }

void CommandPool::Init()
{
	const VkResources* vkObj = VkResources::Inst;

	VkCommandPoolCreateInfo poolInfo {};
	poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = vkObj->gpu.GetQueueFamilyIndices().graphicsFamily;
	poolInfo.flags =
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	const VkResult res = vkCreateCommandPool(vkObj->device, &poolInfo, nullptr, &commandPool_);
	vkCheckError(res, "Failed to create command pool!");
}

void CommandPool::Destroy() const
{
	vkDestroyCommandPool(VkResources::Inst->device, commandPool_, nullptr);
}
}    // namespace neko::vk

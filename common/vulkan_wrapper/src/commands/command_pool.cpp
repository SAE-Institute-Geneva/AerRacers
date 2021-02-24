#include "vk/commands/command_pool.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
CommandPool::CommandPool(const std::thread::id& threadId) : threadId_(threadId) { Init(); }

void CommandPool::Init()
{
	const VkResources* vkObj = VkResources::Inst;

	VkCommandPoolCreateInfo poolInfo {};
	poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = vkObj->gpu.GetQueueFamilyIndices().graphicsFamily;
	poolInfo.flags =
		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	const VkResult res =
		vkCreateCommandPool(VkDevice(vkObj->device), &poolInfo, nullptr, &commandPool_);
	neko_assert(res == VK_SUCCESS, "Failed to create command pool!")
}

void CommandPool::Destroy() const
{
	const VkResources* vkObj = VkResources::Inst;
	vkDestroyCommandPool(VkDevice(vkObj->device), commandPool_, nullptr);
}
}    // namespace neko::vk

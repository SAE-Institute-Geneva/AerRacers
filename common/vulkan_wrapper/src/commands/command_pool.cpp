#include "vk/commands/command_pool.h"

namespace neko::vk
{
CommandPool::CommandPool(const PhysicalDevice& gpu, const LogicalDevice& device) : gpu_(gpu), device_(device)
{
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = gpu_.GetQueueFamilyIndices().graphicsFamily;
	poolInfo.flags = 0; // Optional

	const VkResult res = vkCreateCommandPool(VkDevice(device_), &poolInfo, nullptr, &commandPool_);
	neko_assert(res == VK_SUCCESS, "Failed to create command pool!")
}

CommandPool::~CommandPool()
{
	vkDestroyCommandPool(VkDevice(device_), commandPool_, nullptr);
}
}

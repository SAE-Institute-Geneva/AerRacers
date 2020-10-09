#include "vk/commands/command_pool.h"

namespace neko::vk
{
void CommandPool::Init(const PhysicalDevice& gpu, const LogicalDevice& device)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = gpu.GetQueueFamilyIndices().graphicsFamily;
    poolInfo.flags = 0; // Optional

    const VkResult res = vkCreateCommandPool(VkDevice(device), &poolInfo, nullptr, &commandPool_);
    neko_assert(res == VK_SUCCESS, "Failed to create command pool!")
}

void CommandPool::Destroy(const LogicalDevice& device) const
{
    vkDestroyCommandPool(VkDevice(device), commandPool_, nullptr);
}
}

#include "vk/commands/command_pool.h"

namespace neko::vk
{
CommandPool::CommandPool(const LogicalDevice& device) : device_(device)
{}

void CommandPool::Init(const PhysicalDevice& gpu)
{
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = gpu.GetQueueFamilyIndices().graphicsFamily;
    poolInfo.flags = 0; // Optional

    const VkResult res = vkCreateCommandPool(VkDevice(device_), &poolInfo, nullptr, &commandPool_);
    neko_assert(res == VK_SUCCESS, "Failed to create command pool!")
}

void CommandPool::Destroy()
{
    vkDestroyCommandPool(VkDevice(device_), commandPool_, nullptr);
}
}

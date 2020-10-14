#include "vk/commands/command_pool.h"
#include "vk/graphics.h"

namespace neko::vk
{
void CommandPool::Init()
{
    const auto& vkObj = VkResourcesLocator::get();

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = vkObj.gpu.GetQueueFamilyIndices().graphicsFamily;
    poolInfo.flags = 0; // Optional

    const VkResult res = vkCreateCommandPool(VkDevice(vkObj.device), &poolInfo, nullptr, &commandPool_);
    neko_assert(res == VK_SUCCESS, "Failed to create command pool!")
}

void CommandPool::Destroy() const
{
    const auto& vkObj = VkResourcesLocator::get();
    vkDestroyCommandPool(VkDevice(vkObj.device), commandPool_, nullptr);
}
}

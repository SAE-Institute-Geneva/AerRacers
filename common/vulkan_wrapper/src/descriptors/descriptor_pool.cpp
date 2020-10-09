#include "vk/descriptors/descriptor_pool.h"

namespace neko::vk
{
void DescriptorPool::Init(const LogicalDevice& device, const Swapchain& swapchain)
{
    const auto swapchainImagesCount = static_cast<uint32_t>(swapchain.GetImageCount());

    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = swapchainImagesCount;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = swapchainImagesCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = swapchainImagesCount;

    const VkResult res = vkCreateDescriptorPool(VkDevice(device), &poolInfo, nullptr, &descriptorPool_);
    neko_assert(res == VK_SUCCESS, "Failed to create descriptor pool!")
}

void DescriptorPool::Destroy(const LogicalDevice& device) const
{
    vkDestroyDescriptorPool(VkDevice(device), descriptorPool_, nullptr);
}
}
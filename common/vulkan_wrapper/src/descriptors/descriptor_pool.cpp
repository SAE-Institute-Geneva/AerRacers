#include "vk/descriptors/descriptor_pool.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
void DescriptorPool::Init()
{
	const VkResources* vkObj        = VkResources::Inst;
	const auto swapchainImagesCount = vkObj->swapchain.GetImageCount();

	std::array<VkDescriptorPoolSize, 2> poolSizes {};
	poolSizes[0].type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = swapchainImagesCount;
	poolSizes[1].type            = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = swapchainImagesCount;

	VkDescriptorPoolCreateInfo poolInfo {};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<std::uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes    = poolSizes.data();
	poolInfo.maxSets       = swapchainImagesCount;

	vkCheckError(vkCreateDescriptorPool(vkObj->device, &poolInfo, nullptr, &descriptorPool_),
		"Failed to create descriptor pool!");
}

void DescriptorPool::Destroy() const
{
	vkDestroyDescriptorPool(VkResources::Inst->device, descriptorPool_, nullptr);
}
}    // namespace neko::vk
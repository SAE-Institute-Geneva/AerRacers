#include "vk/vk_resources.h"

namespace neko::vk
{
DescriptorSet::DescriptorSet(const Pipeline& pipeline)
   : pipelineLayout_(pipeline.GetPipelineLayout()),
	 pipelineBindPoint_(pipeline.GetPipelineBindPoint()),
	 descriptorPool_(pipeline.GetDescriptorPool())
{
	VkDescriptorSetLayout layouts[1] = {pipeline.GetDescriptorSetLayout()};

	VkDescriptorSetAllocateInfo descriptorSetAllocateInfo {};
	descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorSetAllocateInfo.descriptorPool     = descriptorPool_;
	descriptorSetAllocateInfo.descriptorSetCount = 1;
	descriptorSetAllocateInfo.pSetLayouts        = layouts;

	vkCheckError(vkAllocateDescriptorSets(
					 VkResources::Inst->device, &descriptorSetAllocateInfo, &descriptorSet_),
		"Failed to create descriptor set layout!");
}

void DescriptorSet::Update(const std::vector<VkWriteDescriptorSet>& descriptors)
{
	vkUpdateDescriptorSets(VkResources::Inst->device,
		static_cast<std::uint32_t>(descriptors.size()),
		descriptors.data(),
		0,
		nullptr);
}

void DescriptorSet::Destroy() const
{
	vkFreeDescriptorSets(VkResources::Inst->device, descriptorPool_, 1, &descriptorSet_);
}

void DescriptorSet::BindDescriptor(const CommandBuffer& commandBuffer) const
{
	vkCmdBindDescriptorSets(
		commandBuffer, pipelineBindPoint_, pipelineLayout_, 0, 1, &descriptorSet_, 0, nullptr);
}
}    // namespace neko::vk

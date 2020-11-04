#include "vk/descriptors/descriptor_set.h"

namespace neko::vk
{
WriteDescriptorSet::WriteDescriptorSet(
        VkWriteDescriptorSet writeDescriptorSet,
        VkDescriptorImageInfo imageInfo)
        : writeDescriptorSet_(writeDescriptorSet),
        imageInfo_(imageInfo)
{
    if (imageInfo_.imageView) writeDescriptorSet_.pImageInfo = &imageInfo_;
}

WriteDescriptorSet::WriteDescriptorSet(
        VkWriteDescriptorSet writeDescriptorSet,
        VkDescriptorBufferInfo bufferInfo)
        : writeDescriptorSet_(writeDescriptorSet),
          bufferInfo_(bufferInfo)
{
    writeDescriptorSet_.pBufferInfo = &bufferInfo_;
}

VkWriteDescriptorSet WriteDescriptorSet::GetWriteDescriptorSet()
{
    if (imageInfo_.imageView) writeDescriptorSet_.pImageInfo = &imageInfo_;
    else if (bufferInfo_.buffer) writeDescriptorSet_.pBufferInfo = &bufferInfo_;

    return writeDescriptorSet_;
}

DescriptorSet::DescriptorSet(const Pipeline& pipeline)
    : pipelineLayout_(pipeline.GetPipelineLayout()),
    pipelineBindPoint_(pipeline.GetPipelineBindPoint()),
    descriptorPool_(pipeline.GetDescriptorPool())
{
    const auto& device = VkDevice(VkObjectsLocator::get().device);
    VkDescriptorSetLayout layouts[1] = {pipeline.GetDescriptorSetLayout()};

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{};
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool_;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = layouts;

    const VkResult res = vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &descriptorSet_);
    neko_assert(res == VK_SUCCESS, "Failed to create descriptor set layout!")
}

void DescriptorSet::Update(const std::vector<VkWriteDescriptorSet>& descriptors)
{
    const auto& device = VkDevice(VkObjectsLocator::get().device);
    vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptors.size()),
            descriptors.data(), 0, nullptr);
}

void DescriptorSet::Destroy() const
{
    const auto& device = VkDevice(VkObjectsLocator::get().device);
    vkFreeDescriptorSets(VkDevice(device), descriptorPool_, 1, &descriptorSet_);
}

void DescriptorSet::BindDescriptor(const CommandBuffer& commandBuffer) const
{
    vkCmdBindDescriptorSets(VkCommandBuffer(commandBuffer), pipelineBindPoint_,
                            pipelineLayout_, 0, 1, &descriptorSet_, 0, nullptr);
}
}
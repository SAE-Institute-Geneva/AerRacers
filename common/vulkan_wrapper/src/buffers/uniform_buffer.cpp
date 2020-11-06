#include "vk/buffers/uniform_buffer.h"

namespace neko::vk
{
UniformBuffer::UniformBuffer(const VkDeviceSize size, 
    const std::vector<char>& uniformData)
        : Buffer(size,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
        VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        uniformData.data()) {}

void UniformBuffer::Update(const std::vector<char>& newUniformData) const
{
    char* dataPtr;
    MapMemory(&dataPtr);
        memcpy(dataPtr, newUniformData.data(), static_cast<std::size_t>(size_));
    UnmapMemory();
}

void UniformBuffer::Destroy() const
{
    const auto& vkObj = VkObjectsLocator::get();
	
	vkDestroyBuffer(VkDevice(vkObj.device), buffer_, nullptr);
	vkFreeMemory(VkDevice(vkObj.device), memory_, nullptr);
}

VkDescriptorSetLayoutBinding UniformBuffer::GetDescriptorSetLayout(
	const std::uint32_t binding,
	const VkDescriptorType descriptorType,
	const VkShaderStageFlags stage)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = binding;
    uboLayoutBinding.descriptorType = descriptorType;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = stage;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    return uboLayoutBinding;
}

WriteDescriptorSet UniformBuffer::GetWriteDescriptor(
	const std::uint32_t binding,
	const VkDescriptorType descriptorType) const
{
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = buffer_;
    bufferInfo.offset = 0;
    bufferInfo.range = size_;

    VkWriteDescriptorSet descriptorWrite{};
    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = {};
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.descriptorType = descriptorType;
    return WriteDescriptorSet(descriptorWrite, bufferInfo);
}
}
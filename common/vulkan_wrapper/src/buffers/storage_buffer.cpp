#include "vk/buffers/storage_buffer.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
StorageBuffer::StorageBuffer(const VkDeviceSize& size, const std::vector<char>& data)
   : Buffer(size,
		 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		 data.data())
{}

void StorageBuffer::Update(const std::vector<char>& newStorageData) const
{
	char* dataPtr;
	MapMemory(&dataPtr);
	memcpy(dataPtr, newStorageData.data(), static_cast<std::size_t>(size_));
	UnmapMemory();
}

void StorageBuffer::Destroy() const
{
	const auto& device = VkDevice(VkResources::Inst->device);

	if (buffer_) vkDestroyBuffer(device, buffer_, nullptr);
	if (memory_) vkFreeMemory(device, memory_, nullptr);
}

VkDescriptorSetLayoutBinding StorageBuffer::GetDescriptorSetLayout(const std::uint32_t binding,
	const VkDescriptorType descriptorType,
	const VkShaderStageFlags stage)
{
	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
	descriptorSetLayoutBinding.binding            = binding;
	descriptorSetLayoutBinding.descriptorType     = descriptorType;
	descriptorSetLayoutBinding.descriptorCount    = 1;
	descriptorSetLayoutBinding.stageFlags         = stage;
	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
	return descriptorSetLayoutBinding;
}

WriteDescriptorSet StorageBuffer::GetWriteDescriptor(
	const std::uint32_t binding, const VkDescriptorType descriptorType) const
{
	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = buffer_;
	bufferInfo.offset = 0;
	bufferInfo.range  = size_;

	VkWriteDescriptorSet descriptorWrite {};
	descriptorWrite.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet          = {};
	descriptorWrite.dstBinding      = binding;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.descriptorType  = descriptorType;
	return WriteDescriptorSet(descriptorWrite, bufferInfo);
}
}    // namespace neko::vk

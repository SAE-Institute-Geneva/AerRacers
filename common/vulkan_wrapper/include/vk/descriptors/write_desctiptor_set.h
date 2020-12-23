#pragma once
#include "vk/vulkan_include.h"

namespace neko::vk
{
class WriteDescriptorSet
{
public:
    WriteDescriptorSet(
            VkWriteDescriptorSet writeDescriptorSet,
            VkDescriptorImageInfo imageInfo);

	WriteDescriptorSet(
            VkWriteDescriptorSet writeDescriptorSet,
            VkDescriptorBufferInfo bufferInfo);

	VkWriteDescriptorSet GetWriteDescriptorSet();

private:
    VkWriteDescriptorSet writeDescriptorSet_{};
	VkDescriptorImageInfo imageInfo_{};
	VkDescriptorBufferInfo bufferInfo_{};
};
}
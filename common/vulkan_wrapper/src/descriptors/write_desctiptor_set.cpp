#include "vk/descriptors/write_desctiptor_set.h"

namespace neko::vk
{
WriteDescriptorSet::WriteDescriptorSet(
	const VkWriteDescriptorSet& writeDescriptorSet, const VkDescriptorImageInfo& imageInfo)
   : writeDescriptorSet_(writeDescriptorSet), imageInfo_(imageInfo)
{
	if (imageInfo_.imageView) writeDescriptorSet_.pImageInfo = &imageInfo_;
}

WriteDescriptorSet::WriteDescriptorSet(
	const VkWriteDescriptorSet& writeDescriptorSet, const VkDescriptorBufferInfo& bufferInfo)
   : writeDescriptorSet_(writeDescriptorSet), bufferInfo_(bufferInfo)
{
	writeDescriptorSet_.pBufferInfo = &bufferInfo_;
}

VkWriteDescriptorSet WriteDescriptorSet::GetWriteDescriptorSet()
{
	if (imageInfo_.imageView) writeDescriptorSet_.pImageInfo = &imageInfo_;
	else if (bufferInfo_.buffer) writeDescriptorSet_.pBufferInfo = &bufferInfo_;

	return writeDescriptorSet_;
}
}    // namespace neko::vk

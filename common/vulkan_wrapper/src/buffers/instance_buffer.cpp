#include "vk/buffers/instance_buffer.h"

namespace neko::vk
{
InstanceBuffer::InstanceBuffer(const VkDeviceSize size)
		: Buffer(size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {}

void InstanceBuffer::Update(const std::vector<char>& newInstanceData) const
{
	char* instanceDataPtr;
	MapMemory(&instanceDataPtr);
	memcpy(instanceDataPtr, newInstanceData.data(), static_cast<size_t>(size_));
	UnmapMemory();
}
}
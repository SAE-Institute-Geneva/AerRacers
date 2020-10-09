#include "vk/buffers/uniform_buffer.h"

namespace neko::vk
{
void UniformBuffer::Init(const PhysicalDevice& gpu, const LogicalDevice& device)
{
	const VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffer_, memory_);
}
}
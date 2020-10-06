#include "vk/buffers/uniform_buffer.h"

namespace neko::vk
{
UniformBuffer::UniformBuffer(const LogicalDevice& device) : Buffer(device)
{}

void UniformBuffer::Init(const PhysicalDevice& gpu)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device_), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffer_, memory_);
}
}
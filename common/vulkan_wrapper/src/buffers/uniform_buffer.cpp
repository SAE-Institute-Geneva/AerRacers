#include "vk/buffers/uniform_buffer.h"

namespace neko::vk
{
void UniformBuffer::Init(const PhysicalDevice& gpu, const VkDeviceSize& uboSize)
{
    const auto& device = LogicalDeviceLocator::get();

    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device), uboSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 buffer_, memory_);
}
}
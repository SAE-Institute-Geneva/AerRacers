#include "vk/buffers/index_buffer.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
IndexBuffer::IndexBuffer(const LogicalDevice& device) : Buffer(device)
{}

const std::vector<uint16_t> indices = {
        0, 1, 2, 2, 3, 0
};

void IndexBuffer::Init(const PhysicalDevice& gpu, const CommandPool& commandPool)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device_), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    vkMapMemory(VkDevice(device_), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(VkDevice(device_), stagingBufferMemory);

    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device_), bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_, memory_);

    const auto& graphicsQueue = device_.GetGraphicsQueue();
    CopyBuffer(graphicsQueue, VkDevice(device_), VkCommandPool(commandPool), stagingBuffer, buffer_, bufferSize);

    vkDestroyBuffer(VkDevice(device_), stagingBuffer, nullptr);
    vkFreeMemory(VkDevice(device_), stagingBufferMemory, nullptr);
}
}

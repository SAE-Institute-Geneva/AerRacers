#include "vk/buffers/index_buffer.h"

namespace neko::vk
{
const std::vector<uint16_t> kIndices = {
        0, 1, 2, 2, 3, 0
};

void IndexBuffer::Init(const PhysicalDevice& gpu, const LogicalDevice& device, const CommandPool& commandPool)
{
	const VkDeviceSize bufferSize = sizeof(kIndices[0]) * kIndices.size();

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    vkMapMemory(VkDevice(device), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, kIndices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(VkDevice(device), stagingBufferMemory);

    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device), bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_, memory_);

    const auto& graphicsQueue = device.GetGraphicsQueue();
    CopyBuffer(graphicsQueue, VkDevice(device), VkCommandPool(commandPool), stagingBuffer, buffer_, bufferSize);

    vkDestroyBuffer(VkDevice(device), stagingBuffer, nullptr);
    vkFreeMemory(VkDevice(device), stagingBufferMemory, nullptr);
}
}

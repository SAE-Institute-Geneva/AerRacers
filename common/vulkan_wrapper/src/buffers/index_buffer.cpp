#include "vk/buffers/index_buffer.h"

#include <cstring>

namespace neko::vk
{
const std::array<std::uint16_t, 6> kIndices = {
        0, 1, 2, 2, 3, 0
};

void IndexBuffer::Init(const std::uint16_t indices[], const std::size_t indexNum)
{
    const auto& vkObj = VkObjectsLocator::get();

	const VkDeviceSize bufferSize = sizeof(std::uint16_t) * indexNum;

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    CreateBuffer(VkPhysicalDevice(vkObj.gpu), VkDevice(vkObj.device), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    vkMapMemory(VkDevice(vkObj.device), stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices, static_cast<std::size_t>(bufferSize));
    vkUnmapMemory(VkDevice(vkObj.device), stagingBufferMemory);

    CreateBuffer(VkPhysicalDevice(vkObj.gpu), VkDevice(vkObj.device), bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_, memory_);

    const auto& graphicsQueue = vkObj.device.GetGraphicsQueue();
    CopyBuffer(graphicsQueue, VkDevice(vkObj.device), VkCommandPool(*vkObj.commandPools),
               stagingBuffer, buffer_, bufferSize);

    vkDestroyBuffer(VkDevice(vkObj.device), stagingBuffer, nullptr);
    vkFreeMemory(VkDevice(vkObj.device), stagingBufferMemory, nullptr);
}
}

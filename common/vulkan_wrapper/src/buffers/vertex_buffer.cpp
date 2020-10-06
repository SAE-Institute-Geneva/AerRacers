#include "vk/buffers/vertex_buffer.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
VertexBuffer::VertexBuffer(const LogicalDevice& device)
        : Buffer(device)
{}

const std::vector<Vertex> vertices =
{
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f,  -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f,   0.5f, 0.0f},  {0.0f, 0.0f, 1.0f}},
        {{-0.5f,  0.5f, 0.0f},  {1.0f, 1.0f, 1.0f}}
};

void VertexBuffer::Init(const PhysicalDevice& gpu, const CommandPool& commandPool)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device_), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    vkMapMemory(VkDevice(device_), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
    vkUnmapMemory(VkDevice(device_), stagingBufferMemory);

    CreateBuffer(VkPhysicalDevice(gpu), VkDevice(device_), bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_, memory_);

    const auto& graphicsQueue = device_.GetGraphicsQueue();
    CopyBuffer(graphicsQueue, VkDevice(device_), VkCommandPool(commandPool), stagingBuffer, buffer_, bufferSize);

    vkDestroyBuffer(VkDevice(device_), stagingBuffer, nullptr);
    vkFreeMemory(VkDevice(device_), stagingBufferMemory, nullptr);
}
}
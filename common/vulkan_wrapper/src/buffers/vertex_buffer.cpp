#include "vk/buffers/vertex_buffer.h"

#include <utility>
#include <vk/models/mesh.h>


namespace neko::vk
{
Vertex::Vertex(const Vec3f& pos, const Vec3f& norm, const Vec2f& uv)
    : position(pos), normal(norm), texCoords(uv)
{}

VertexInput Vertex::GetVertexInput(uint32_t binding)
{
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
    attributeDescriptions[0].binding = binding;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = binding;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);

    attributeDescriptions[2].binding = binding;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoords);

    return VertexInput(binding, bindingDescription, attributeDescriptions);
}

VertexInput::VertexInput(uint32_t binding,
    const VkVertexInputBindingDescription& bindingDescriptions,
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions)
    : binding_(binding),
    bindingDescription_(bindingDescriptions),
    attributeDescription_(std::move(attributeDescriptions))
{}

bool VertexInput::operator<(const VertexInput& other) const
{
    return binding_ < other.binding_;
}

void VertexBuffer::Init(const Vertex vertices[], const size_t vertexNum)
{
    const auto& vkObj = VkObjectsLocator::get();

	const VkDeviceSize bufferSize = sizeof(Vertex) * vertexNum;

    VkBuffer stagingBuffer{};
    VkDeviceMemory stagingBufferMemory{};
    CreateBuffer(VkPhysicalDevice(vkObj.gpu), VkDevice(vkObj.device), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                 stagingBuffer, stagingBufferMemory);

    void* data = nullptr;
    vkMapMemory(VkDevice(vkObj.device), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices, static_cast<size_t>(bufferSize));
    vkUnmapMemory(VkDevice(vkObj.device), stagingBufferMemory);

    CreateBuffer(VkPhysicalDevice(vkObj.gpu), VkDevice(vkObj.device), bufferSize,
                 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer_, memory_);

    const auto& graphicsQueue = vkObj.device.GetGraphicsQueue();
    CopyBuffer(graphicsQueue, VkDevice(vkObj.device), VkCommandPool(*vkObj.commandPools),
               stagingBuffer, buffer_, bufferSize);

    vkDestroyBuffer(VkDevice(vkObj.device), stagingBuffer, nullptr);
    vkFreeMemory(VkDevice(vkObj.device), stagingBufferMemory, nullptr);
}
}
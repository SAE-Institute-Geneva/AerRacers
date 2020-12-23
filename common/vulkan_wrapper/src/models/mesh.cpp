#include "vk/models/mesh.h"

namespace neko::vk
{
VertexInput::VertexInput(
        std::uint32_t binding,
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

Vertex::Vertex(const Vec3f& pos, const Vec3f& norm, const Vec2f& uv)
        : position(pos), normal(norm), texCoords(uv)
{}

VertexInput Vertex::GetVertexInput(std::uint32_t binding)
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

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
    : Mesh() { InitData(vertices, indices); }

void Mesh::InitData(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
{
    SetVertices(vertices);
    SetIndices(indices);

    minExtents_ = Vec3f(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max());
    maxExtents_ = Vec3f(
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min(),
            std::numeric_limits<float>::min());

    for (const auto& vertex : vertices)
    {
        minExtents_ = Vec3f(
                std::min(minExtents_.x, vertex.position.x),
                std::min(minExtents_.y, vertex.position.y),
                std::min(minExtents_.z, vertex.position.z));
        maxExtents_ = Vec3f(
                std::max(maxExtents_.x, vertex.position.x),
                std::max(maxExtents_.y, vertex.position.y),
                std::max(maxExtents_.z, vertex.position.z));
    }

    radius_ = std::max(
            minExtents_.Magnitude(),
            maxExtents_.Magnitude());
}

void Mesh::Destroy() const
{
	if (vertexBuffer_) vertexBuffer_->Destroy();
	if (indexBuffer_) indexBuffer_->Destroy();
}

void Mesh::Init()
{
    InitData(GetVertices(0), GetIndices(0));
}

bool Mesh::CmdRender(const CommandBuffer& commandBuffer, const std::uint32_t instance) const
{
    if (!vertexBuffer_) return false;

    VkBuffer vertexBuffers[] = {vertexBuffer_->GetBuffer()};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(VkCommandBuffer(commandBuffer), 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(VkCommandBuffer(commandBuffer),
            indexBuffer_->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdDrawIndexed(VkCommandBuffer(commandBuffer), indexCount_, instance, 0, 0, 0);

    return true;
}

std::vector<Vertex> Mesh::GetVertices(const std::size_t offset) const
{
    if (!vertexBuffer_) return {};
    const Buffer vertexStaging(
            vertexBuffer_->GetSize(),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    CommandBuffer commandBuffer;
    commandBuffer.Init();

    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexStaging.GetSize();
    vkCmdCopyBuffer(VkCommandBuffer(commandBuffer), vertexBuffer_->GetBuffer(),
            vertexStaging.GetBuffer(), 1, &copyRegion);

    commandBuffer.SubmitIdle();

    char* verticesMemory;
    vertexStaging.MapMemory(&verticesMemory);
    std::vector<Vertex> vertices(vertexCount_);

    const auto sizeOfSrcT = vertexStaging.GetSize() / vertexCount_;

    for (std::uint32_t i = 0; i < vertexCount_; i++)
    {
        memcpy(&vertices[i],
               static_cast<char *>(verticesMemory) + (i * sizeOfSrcT) + offset,
               sizeof(Vertex));
    }

    vertexStaging.UnmapMemory();
    return vertices;
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices)
{
    vertexBuffer_.reset();
    vertexCount_ = static_cast<std::uint32_t>(vertices.size());

    if (vertices.empty()) return;

    const auto vertexStaging = Buffer(
            sizeof(Vertex) * vertices.size(),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertices.data());

    vertexBuffer_.emplace(
            vertexStaging.GetSize(),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    CommandBuffer commandBuffer;
    commandBuffer.Init();

    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexStaging.GetSize();
    vkCmdCopyBuffer(VkCommandBuffer(commandBuffer), vertexStaging.GetBuffer(),
                    vertexBuffer_->GetBuffer(), 1, &copyRegion);

    commandBuffer.SubmitIdle();
	vertexStaging.Destroy();
}

std::vector<std::uint32_t> Mesh::GetIndices(const std::size_t offset) const
{
    if (!indexBuffer_) return {};

    const Buffer indexStaging(indexBuffer_->GetSize(),
                              VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    CommandBuffer commandBuffer;
    commandBuffer.Init();

    VkBufferCopy copyRegion{};
    copyRegion.size = indexStaging.GetSize();
    vkCmdCopyBuffer(VkCommandBuffer(commandBuffer), indexBuffer_->GetBuffer(),
                    indexStaging.GetBuffer(), 1, &copyRegion);

    commandBuffer.SubmitIdle();

    char *indicesMemory;
    indexStaging.MapMemory(&indicesMemory);
    std::vector<std::uint32_t> indices(indexCount_);

    const auto sizeOfSrcT = indexStaging.GetSize() / indexCount_;

    for (std::uint32_t i = 0; i < indexCount_; i++)
    {
        memcpy(&indices[i],
               static_cast<char *>(indicesMemory) + (i * sizeOfSrcT) + offset,
               sizeof(std::uint32_t));
    }

    indexStaging.UnmapMemory();
    return indices;
}

void Mesh::SetIndices(const std::vector<std::uint32_t>& indices)
{
    indexBuffer_.reset();
    indexCount_ = static_cast<std::uint32_t>(indices.size());

    if (indices.empty()) return;

    const auto indexStaging = Buffer(
            sizeof(std::uint32_t) * indices.size(),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            indices.data());

    indexBuffer_.emplace(
            indexStaging.GetSize(),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    auto commandBuffer = CommandBuffer();
    commandBuffer.Init();

    VkBufferCopy copyRegion{};
    copyRegion.size = indexStaging.GetSize();
    vkCmdCopyBuffer(VkCommandBuffer(commandBuffer), indexStaging.GetBuffer(),
            indexBuffer_->GetBuffer(), 1, &copyRegion);

    commandBuffer.SubmitIdle();

	indexStaging.Destroy();
}

Vec3f Mesh::GetExtent() const
{
    const Vec3f extent =
    {
            abs(maxExtents_.x) + abs(minExtents_.x),
            abs(maxExtents_.y) + abs(minExtents_.y),
            abs(maxExtents_.z) + abs(minExtents_.z)
    };

    return extent;
}
}
#pragma once
#include "optional.hpp"

#include "vk/buffers/buffer.h"
#include "vk/commands/command_buffer.h"

namespace neko::vk
{
class VertexInput
{
public:
    explicit VertexInput(std::uint32_t binding = 0,
                         const VkVertexInputBindingDescription& bindingDescriptions = {},
                         std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {});

    [[nodiscard]] std::uint32_t GetBinding() const { return binding_; }

    [[nodiscard]] const VkVertexInputBindingDescription&
    GetBindingDescription() const { return bindingDescription_; }

    [[nodiscard]] const std::vector<VkVertexInputAttributeDescription>&
    GetAttributeDescriptions() const { return attributeDescription_; }

    //To sort by bindings
    bool operator<(const VertexInput& other) const;

private:
    std::uint32_t binding_;
    VkVertexInputBindingDescription bindingDescription_;
    std::vector<VkVertexInputAttributeDescription> attributeDescription_;
};

struct Vertex
{
    explicit Vertex() = default;
    Vertex(const Vec3f& pos, const Vec3f& norm, const Vec2f& uv);
    static VertexInput GetVertexInput(std::uint32_t binding = 0);

    Vec3f position = Vec3f::zero;
    Vec3f normal = Vec3f::zero;
    Vec2f texCoords = Vec2f::zero;
};

class Mesh
{
public:
    Mesh() = default;
    virtual ~Mesh() = default;
    explicit Mesh(const std::vector<Vertex>& vertices,
            const std::vector<std::uint32_t>& indices = {});

    virtual void Init();
    void InitData(const std::vector<Vertex>& vertices,
                          const std::vector<std::uint32_t>& indices);
	void Destroy() const;

    [[nodiscard]] bool CmdRender(const CommandBuffer& commandBuffer, std::uint32_t instance = 1) const;

    [[nodiscard]] virtual std::vector<Vertex> GetVertices(std::size_t offset) const;
    virtual void SetVertices(const std::vector<Vertex>& vertices);

    [[nodiscard]] virtual std::vector<uint32_t> GetIndices(std::size_t offset) const;
    virtual void SetIndices(const std::vector<uint32_t>& indices);

    [[nodiscard]] const Buffer& GetVertexBuffer() const { return *vertexBuffer_; }
    [[nodiscard]] const Buffer& GetIndexBuffer() const { return *indexBuffer_; }

    [[nodiscard]] std::uint32_t GetVertexCount() const { return vertexCount_; }
    [[nodiscard]] std::uint32_t GetIndexCount() const { return indexCount_; }
    static VkIndexType GetIndexType() { return VK_INDEX_TYPE_UINT32; }

    [[nodiscard]] Vec3f GetExtent() const;
    [[nodiscard]] Vec3f GetPositionOffset() const { return positionOffset_; }

    [[nodiscard]] float GetRadius() const { return radius_; }

protected:
    std::experimental::optional<Buffer> vertexBuffer_;
    std::experimental::optional<Buffer> indexBuffer_;

    Vec3f positionOffset_ = Vec3f::zero;

    Vec3f minExtents_;
    std::uint32_t vertexCount_ = 0;

    Vec3f maxExtents_;
    std::uint32_t indexCount_ = 0;

    float radius_ = 0.0f;
};
}

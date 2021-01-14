#pragma once
#include "vk/buffers/buffer.h"
#include "vk/commands/command_buffer.h"
#include "vk/images/texture_manager.h"
#include "vk/material/diffuse_material.h"
#include "vk/models/vertex_input.h"

namespace neko::vk
{
class Mesh
{
public:
    Mesh() = default;
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

    [[nodiscard]] const Buffer& GetVertexBuffer() const { return vertexBuffer_; }
    [[nodiscard]] const Buffer& GetIndexBuffer() const { return *indexBuffer_; }

    [[nodiscard]] std::uint32_t GetVertexCount() const { return vertexCount_; }
    [[nodiscard]] std::uint32_t GetIndexCount() const { return indexCount_; }
    static VkIndexType GetIndexType() { return VK_INDEX_TYPE_UINT32; }

	[[nodiscard]] ResourceHash GetMaterialId() const { return materialId_; }
	void SetMaterialId(ResourceHash resourceId) { materialId_ = resourceId; }

    [[nodiscard]] Vec3f GetExtent() const;
    [[nodiscard]] Vec3f GetPositionOffset() const { return positionOffset_; }

    [[nodiscard]] float GetRadius() const { return radius_; }

protected:
    Buffer vertexBuffer_{};
    std::neko::optional<Buffer> indexBuffer_ = std::neko::nullopt;

	friend class ModelLoader;
	ResourceHash materialId_ = ResourceHash();

    Vec3f positionOffset_ = Vec3f::zero;

    Vec3f minExtents_;
    std::uint32_t vertexCount_ = 0;

    Vec3f maxExtents_;
    std::uint32_t indexCount_ = 0;

    float radius_ = 0.0f;
};
}

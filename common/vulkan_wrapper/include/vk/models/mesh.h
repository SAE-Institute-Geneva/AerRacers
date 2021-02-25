#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/buffers/buffer.h"
#include "vk/commands/command_buffer.h"
#include "vk/images/texture_manager.h"
#include "vk/models/vertex_input.h"

namespace neko::vk
{
class Mesh
{
public:
	Mesh() = default;
	Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices = {});

	virtual void Init();
	void InitData(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices);
	void Destroy() const;

	[[nodiscard]] bool DrawCmd(
		const CommandBuffer& commandBuffer, std::uint32_t instance = 1) const;

	[[nodiscard]] virtual std::vector<Vertex> GetVertices(std::size_t offset) const;
	virtual void SetVertices(const std::vector<Vertex>& vertices);

	[[nodiscard]] virtual std::vector<std::uint32_t> GetIndices(std::size_t offset) const;
	virtual void SetIndices(const std::vector<std::uint32_t>& indices);

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
	Buffer vertexBuffer_ {};
	std::optional<Buffer> indexBuffer_ = std::nullopt;

	friend class ModelLoader;
	ResourceHash materialId_ = 0;

	Vec3f positionOffset_ = Vec3f::zero;

	Vec3f minExtents_;
	std::uint32_t vertexCount_ = 0;

	Vec3f maxExtents_;
	std::uint32_t indexCount_ = 0;

	float radius_ = 0.0f;
};
}    // namespace neko::vk

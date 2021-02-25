#include "vk/models/mesh.h"

namespace neko::vk
{
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices) : Mesh()
{
	InitData(vertices, indices);
}

void Mesh::InitData(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
{
	SetVertices(vertices);
	SetIndices(indices);

	minExtents_ = Vec3f(std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max(),
		std::numeric_limits<float>::max());
	maxExtents_ = Vec3f(std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min(),
		std::numeric_limits<float>::min());

	for (const auto& vertex : vertices)
	{
		minExtents_ = Vec3f(std::min(minExtents_.x, vertex.position.x),
			std::min(minExtents_.y, vertex.position.y),
			std::min(minExtents_.z, vertex.position.z));
		maxExtents_ = Vec3f(std::max(maxExtents_.x, vertex.position.x),
			std::max(maxExtents_.y, vertex.position.y),
			std::max(maxExtents_.z, vertex.position.z));
	}

	radius_ = std::max(minExtents_.Magnitude(), maxExtents_.Magnitude());
}

void Mesh::Destroy() const
{
	vertexBuffer_.Destroy();
	if (indexBuffer_) indexBuffer_->Destroy();
}

void Mesh::Init() { InitData(GetVertices(0), GetIndices(0)); }

bool Mesh::DrawCmd(const CommandBuffer& commandBuffer, const std::uint32_t instance) const
{
	VkBuffer vertexBuffers[] = {vertexBuffer_.GetBuffer()};
	VkDeviceSize offsets[]   = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, indexCount_, instance, 0, 0, 0);

	return true;
}

std::vector<Vertex> Mesh::GetVertices(const std::size_t offset) const
{
	const Buffer vertexStaging(vertexBuffer_.GetSize(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	CommandBuffer commandBuffer(true);
	{
		VkBufferCopy copyRegion = {};
		copyRegion.size         = vertexStaging.GetSize();
		vkCmdCopyBuffer(commandBuffer, vertexBuffer_, vertexStaging, 1, &copyRegion);
	}
	commandBuffer.SubmitIdle();

	char* verticesMemory;
	std::vector<Vertex> vertices(vertexCount_);
	vertexStaging.MapMemory(&verticesMemory);
	{
		const std::size_t sizeOfSrcT = vertexStaging.GetSize() / vertexCount_;
		for (std::uint32_t i = 0; i < vertexCount_; i++)
		{
			std::memcpy(&vertices[i],
				static_cast<char*>(verticesMemory) + (i * sizeOfSrcT) + offset,
				sizeof(Vertex));
		}
	}
	vertexStaging.UnmapMemory();
	return vertices;
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices)
{
	if (VkBuffer(vertexBuffer_)) vertexBuffer_.Destroy();
	vertexCount_ = static_cast<std::uint32_t>(vertices.size());
	if (vertices.empty()) return;

	const auto vertexStaging = Buffer(sizeof(Vertex) * vertices.size(),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vertices.data());

	vertexBuffer_ = Buffer(vertexStaging.GetSize(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CommandBuffer commandBuffer(true);
	{
		VkBufferCopy copyRegion = {};
		copyRegion.size         = vertexStaging.GetSize();
		vkCmdCopyBuffer(commandBuffer, vertexStaging, vertexBuffer_, 1, &copyRegion);
	}
	commandBuffer.SubmitIdle();
	vertexStaging.Destroy();
}

std::vector<std::uint32_t> Mesh::GetIndices(const std::size_t offset) const
{
	if (!indexBuffer_) return {};

	const Buffer indexStaging(indexBuffer_->GetSize(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	CommandBuffer commandBuffer(true);
	{
		VkBufferCopy copyRegion {};
		copyRegion.size = indexStaging.GetSize();
		vkCmdCopyBuffer(commandBuffer, indexBuffer_.value(), indexStaging, 1, &copyRegion);
	}
	commandBuffer.SubmitIdle();

	char* indicesMemory;
	std::vector<std::uint32_t> indices(indexCount_);
	indexStaging.MapMemory(&indicesMemory);
	{
		const std::size_t sizeOfSrcT = indexStaging.GetSize() / indexCount_;
		for (std::uint32_t i = 0; i < indexCount_; i++)
		{
			std::memcpy(&indices[i],
				static_cast<char*>(indicesMemory) + (i * sizeOfSrcT) + offset,
				sizeof(std::uint32_t));
		}
	}
	indexStaging.UnmapMemory();
	return indices;
}

void Mesh::SetIndices(const std::vector<std::uint32_t>& indices)
{
	indexBuffer_.reset();
	indexCount_ = static_cast<std::uint32_t>(indices.size());
	if (indices.empty()) return;

	const auto indexStaging = Buffer(sizeof(std::uint32_t) * indices.size(),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		indices.data());

	indexBuffer_.emplace(indexStaging.GetSize(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	CommandBuffer commandBuffer(true);
	{
		VkBufferCopy copyRegion {};
		copyRegion.size = indexStaging.GetSize();
		vkCmdCopyBuffer(commandBuffer, indexStaging, indexBuffer_.value(), 1, &copyRegion);
	}
	commandBuffer.SubmitIdle();
	indexStaging.Destroy();
}

Vec3f Mesh::GetExtent() const
{
	const Vec3f extent = {abs(maxExtents_.x) + abs(minExtents_.x),
		abs(maxExtents_.y) + abs(minExtents_.y),
		abs(maxExtents_.z) + abs(minExtents_.z)};

	return extent;
}
}    // namespace neko::vk
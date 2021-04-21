#include "vk/ui/quad_ui.h"

namespace neko::vk
{
void RenderQuadUi::Init()
{
	InitVertices();
	InitIndices();
}

void RenderQuadUi::InitVertices()
{
	const std::array<Vec4f, 4> vertices = {
		Vec4f(-0.5f,  0.5f, 0.0f, 1.0f),    // Top Left
		Vec4f(-0.5f, -0.5f, 0.0f, 0.0f),    // Bottom Left
		Vec4f( 0.5f, -0.5f, 1.0f, 0.0f),    // Bottom Right
		Vec4f( 0.5f,  0.5f, 1.0f, 1.0f),    // Top Right
	};

	if (VkBuffer(vertexBuffer_)) vertexBuffer_.Destroy();
	const auto vertexStaging = Buffer(sizeof(Vertex) * vertices.size(),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		vertices.data());

	vertexBuffer_ = {vertexStaging.GetSize(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};

	CommandBuffer commandBuffer(true);
	{
		VkBufferCopy copyRegion = {};
		copyRegion.size         = vertexStaging.GetSize();
		vkCmdCopyBuffer(commandBuffer, vertexStaging, vertexBuffer_, 1, &copyRegion);
	}

	commandBuffer.SubmitIdle();
	vertexStaging.Destroy();
}

void RenderQuadUi::InitIndices()
{
	const std::array<std::uint32_t, 6> indices = {0, 1, 3, 1, 2, 3};

	if (VkBuffer(indexBuffer_)) indexBuffer_.Destroy();
	const Buffer indexStaging = {sizeof(std::uint32_t) * indices.size(),
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		indices.data()};

	indexBuffer_ = {indexStaging.GetSize(),
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT};

	CommandBuffer commandBuffer(true);
	{
		VkBufferCopy copyRegion {};
		copyRegion.size = indexStaging.GetSize();
		vkCmdCopyBuffer(commandBuffer, indexStaging, indexBuffer_, 1, &copyRegion);
	}

	commandBuffer.SubmitIdle();
	indexStaging.Destroy();
}

bool RenderQuadUi::DrawCmd(const CommandBuffer& commandBuffer, std::uint32_t instance) const
{
	VkBuffer vertexBuffers[] = {vertexBuffer_.GetBuffer()};
	VkDeviceSize offsets[]   = {0};

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer_.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(commandBuffer, 6, instance, 0, 0, 0);

	return true;
}

VertexInput RenderQuadUi::GetVertexInput(std::uint32_t baseBinding)
{
	VkVertexInputBindingDescription bindingDescription {};
	bindingDescription.binding   = baseBinding;
	bindingDescription.stride    = sizeof(Vec4f);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	const std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		{0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, 0},
	};

	return VertexInput(0, bindingDescription, attributeDescriptions);
}
}    // namespace neko::vk
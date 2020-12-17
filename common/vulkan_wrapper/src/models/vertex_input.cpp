#include "vk/models/vertex_input.h"

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
}
#pragma once
#include "mathematics/vector.h"
#include "vk/vulkan_include.h"

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
	Vec3f tangent = Vec3f::zero;
	Vec3f bitangent = Vec3f::zero;
};
}

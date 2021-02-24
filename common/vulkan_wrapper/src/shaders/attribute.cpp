#include "vk/shaders/attribute.h"

namespace neko::vk
{
Attribute::Attribute(
	std::string_view name, std::uint32_t location, std::uint32_t size, Attribute::Type type)
   : name_(std::move(name)), location_(location), size_(size), type_(type)
{}

constexpr VkFormat Attribute::GetVkFormat() const
{
	switch (type_)
	{
		case Type::FLOAT: return VK_FORMAT_R32_SFLOAT;
		case Type::VEC2F: return VK_FORMAT_R32G32_SFLOAT;
		case Type::VEC3F: return VK_FORMAT_R32G32B32_SFLOAT;
		case Type::VEC4F: return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Type::INT:   return VK_FORMAT_R32_SINT;
		case Type::VEC2I: return VK_FORMAT_R32G32_SINT;
		case Type::VEC3I: return VK_FORMAT_R32G32B32_SINT;
		case Type::VEC4I: return VK_FORMAT_R32G32B32A32_SINT;
		case Type::UINT:  return VK_FORMAT_R32_SINT;
		case Type::VEC2U: return VK_FORMAT_R32G32_SINT;
		case Type::VEC3U: return VK_FORMAT_R32G32B32_SINT;
		case Type::VEC4U: return VK_FORMAT_R32G32B32A32_SINT;
		case Type::MAT2:  return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Type::MAT3:  return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Type::MAT4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Type::UNDEFINED:
		default: return VK_FORMAT_UNDEFINED;
	}
}

void Attribute::FromJson(const json& attributeJson)
{
	name_     = attributeJson["name"].get<std::string>();
	location_ = attributeJson["location"].get<std::uint32_t>();
	size_     = attributeJson["size"].get<std::uint32_t>();
	type_     = attributeJson["type"].get<Type>();
}

ordered_json Attribute::ToJson() const
{
	ordered_json attributeJson;
	attributeJson["name"]     = name_;
	attributeJson["location"] = location_;
	attributeJson["size"]     = size_;
	attributeJson["type"]     = type_;

	return attributeJson;
}
}    // namespace neko::vk
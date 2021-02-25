#include "vk/shaders/uniform.h"

#include "mathematics/hash.h"

namespace neko::vk
{
Uniform::Uniform(std::string_view name,
	std::uint32_t binding,
	Type type,
	VkShaderStageFlags stageFlags,
	bool writeOnly)
   : name_(name), binding_(binding), type_(type), stageFlags_(stageFlags), writeOnly_(writeOnly)
{}

Uniform::Uniform(std::string_view name, std::uint32_t offset, std::uint32_t size, bool writeOnly)
   : name_(name), offset_(offset), size_(size), writeOnly_(writeOnly)
{}

Uniform::Uniform(const json& uniformJson) { FromJson(uniformJson); }

bool Uniform::operator==(const Uniform& other) const
{
	return HashString(name_) == HashString(other.name_) && binding_ == other.binding_ &&
	       offset_ == other.offset_ && size_ == other.size_ && type_ == other.type_ &&
	       writeOnly_ == other.writeOnly_ && stageFlags_ == other.stageFlags_;
}

bool Uniform::operator!=(const Uniform& other) const { return !(*this == other); }

void Uniform::FromJson(const json& uniformJson)
{
	name_ = uniformJson["name"].get<std::string_view>();
	if (CheckJsonExists(uniformJson, "binding"))
		binding_ = uniformJson["binding"].get<std::uint32_t>();
	if (CheckJsonExists(uniformJson, "offset"))
		offset_ = uniformJson["offset"].get<std::uint32_t>();
	if (CheckJsonExists(uniformJson, "size"))
		size_ = uniformJson["size"].get<std::uint32_t>();
	if (CheckJsonExists(uniformJson, "type"))
		type_ = uniformJson["type"].get<Type>();
	if (CheckJsonExists(uniformJson, "stageFlags"))
		stageFlags_ = uniformJson["stageFlags"].get<VkShaderStageFlags>();
	writeOnly_ = uniformJson["writeOnly"].get<bool>();
}

ordered_json Uniform::ToJson() const
{
	ordered_json uniformJson;
	uniformJson["name"] = name_;
	if (binding_ != INVALID_INDEX) uniformJson["binding"] = binding_;
	if (offset_ != INVALID_INDEX) uniformJson["offset"] = offset_;
	if (size_ != INVALID_INDEX) uniformJson["size"] = size_;
	if (type_ != Type::UNDEFINED) uniformJson["type"] = type_;
	if (stageFlags_ != 0) uniformJson["stageFlags"] = stageFlags_;
	uniformJson["writeOnly"] = writeOnly_;

	return uniformJson;
}
}    // namespace neko::vk
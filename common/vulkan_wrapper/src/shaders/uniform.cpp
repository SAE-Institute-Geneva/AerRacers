#include "vk/shaders/uniform.h"

namespace neko::vk
{
Uniform::Uniform(
		std::string name,
		const std::uint32_t binding,
		const Type type,
		const VkShaderStageFlags stageFlags,
		const bool readOnly,
		const bool writeOnly)
        : name_(std::move(name)),
        binding_(binding),
        type_(type),
        stageFlags_(stageFlags),
        readOnly_(readOnly),
        writeOnly_(writeOnly)
{}

Uniform::Uniform(
		std::string name,
		const std::uint32_t binding,
		const std::int32_t offset,
		const std::int32_t size,
		const VkShaderStageFlags stageFlags,
		const bool readOnly,
		const bool writeOnly)
        : name_(std::move(name)),
        binding_(binding),
        offset_(offset),
        size_(size),
        stageFlags_(stageFlags),
        readOnly_(readOnly),
        writeOnly_(writeOnly)
{}

Uniform::Uniform(const json& uniformJson)
{
	FromJson(uniformJson);
}

bool Uniform::operator==(const Uniform& other) const
{
	return HashString(name_) == HashString(other.name_) &&
	       binding_ == other.binding_ &&
	       offset_ == other.offset_ &&
	       size_ == other.size_ &&
	       type_ == other.type_ &&
	       readOnly_ == other.readOnly_ &&
	       writeOnly_ == other.writeOnly_ &&
	       stageFlags_ == other.stageFlags_;
}

bool Uniform::operator!=(const Uniform& other) const
{
    return !(*this == other);
}

void Uniform::FromJson(const json& uniformJson)
{
	name_ = uniformJson["name"].get<std::string>();
	binding_ = uniformJson["binding"].get<std::uint32_t>();
	offset_ = uniformJson["offset"].get<std::int32_t>();
	size_ = uniformJson["size"].get<std::int32_t>();
	type_ = uniformJson["type"].get<Type>();
	readOnly_ = uniformJson["readOnly"].get<bool>();
	writeOnly_ = uniformJson["writeOnly"].get<bool>();
	stageFlags_ = uniformJson["stageFlags"].get<VkShaderStageFlags>();
}

ordered_json Uniform::ToJson() const
{
	ordered_json uniformJson;
	uniformJson["name"] = name_;
	uniformJson["binding"] = binding_;
	uniformJson["offset"] = offset_;
	uniformJson["size"] = size_;
	uniformJson["type"] = type_;
	uniformJson["readOnly"] = readOnly_;
	uniformJson["writeOnly"] = writeOnly_;
	uniformJson["stageFlags"] = stageFlags_;

	return uniformJson;
}
}
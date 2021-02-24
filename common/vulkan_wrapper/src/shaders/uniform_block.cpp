#include "vk/shaders/uniform_block.h"

#include "mathematics/hash.h"

namespace neko::vk
{
UniformBlock::UniformBlock(std::string_view name,
	uint32_t binding,
	uint32_t size,
	VkShaderStageFlags stageFlags,
	Type type)
   : name_(std::move(name)), binding_(binding), size_(size), stageFlags_(stageFlags), type_(type)
{}

UniformBlock::UniformBlock(const json& uniformBlockJson) { FromJson(uniformBlockJson); }

void UniformBlock::AddUniform(const Uniform& uniform)
{
	uniforms_.emplace(HashString(uniform.GetName()), uniform);
}

bool UniformBlock::operator==(const UniformBlock& other) const
{
	return HashString(name_) == HashString(other.name_) && binding_ == other.binding_ &&
	       size_ == other.size_ && stageFlags_ == other.stageFlags_ && type_ == other.type_ &&
	       uniforms_ == other.uniforms_;
}

bool UniformBlock::operator!=(const UniformBlock& other) const { return !(*this == other); }

void UniformBlock::FromJson(const json& uniformBlockJson)
{
	name_       = uniformBlockJson["name"].get<std::string>();
	binding_    = uniformBlockJson["binding"].get<std::uint32_t>();
	size_       = uniformBlockJson["size"].get<std::uint32_t>();
	stageFlags_ = uniformBlockJson["stageFlags"].get<VkShaderStageFlags>();
	type_       = uniformBlockJson["type"].get<Type>();

	for (const auto& uniformJson : uniformBlockJson["uniforms"])
	{
		Uniform uniform(uniformJson);
		AddUniform(uniform);
	}
}

ordered_json UniformBlock::ToJson() const
{
	ordered_json uniformBlockJson;
	uniformBlockJson["name"]       = name_;
	uniformBlockJson["binding"]    = binding_;
	uniformBlockJson["size"]       = size_;
	uniformBlockJson["stageFlags"] = stageFlags_;
	uniformBlockJson["type"]       = type_;

	for (const auto& uniform : uniforms_)
	{
		uniformBlockJson["uniforms"].emplace_back(uniform.second.ToJson());
	}

	return uniformBlockJson;
}
}    // namespace neko::vk
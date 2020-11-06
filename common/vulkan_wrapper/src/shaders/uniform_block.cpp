#include "vk/shaders/uniform_block.h"

namespace neko::vk
{
UniformBlock::UniformBlock(
        const std::uint32_t binding,
        const std::uint32_t size,
        const VkShaderStageFlags stageFlags,
        const Type type)
        : binding_(binding),
          size_(size),
          stageFlags_(stageFlags),
          type_(type)
{}

void UniformBlock::AddUniform(XXH64_hash_t uniformHash, const Uniform& uniform)
{
    uniforms_.emplace(std::pair<XXH64_hash_t, Uniform>(uniformHash, uniform));
}

bool UniformBlock::operator==(const UniformBlock& other) const
{
    return binding_ == other.binding_ &&
    size_ == other.size_ &&
    stageFlags_ == other.stageFlags_ &&
    type_ == other.type_ &&
    uniforms_ == other.uniforms_;
}

bool UniformBlock::operator!=(const UniformBlock& other) const
{
    return !(*this == other);
}
}
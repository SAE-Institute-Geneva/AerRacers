#include "vk/shaders/uniform.h"

namespace neko::vk
{
Uniform::Uniform(
        uint32_t binding,
        uint32_t offset,
        uint32_t size,
        uint32_t glType,
        bool readOnly,
        bool writeOnly,
        VkShaderStageFlags stageFlags)
        : binding_(binding),
        offset_(offset),
        size_(size),
        glType_(glType),
        readOnly_(readOnly),
        writeOnly_(writeOnly),
        stageFlags_(stageFlags)
{}

bool Uniform::operator==(const Uniform& other) const
{
    return binding_ == other.binding_ &&
           offset_ == other.offset_ &&
           size_ == other.size_ &&
           glType_ == other.glType_ &&
           readOnly_ == other.readOnly_ &&
           writeOnly_ == other.writeOnly_ &&
           stageFlags_ == other.stageFlags_;
}

bool Uniform::operator!=(const Uniform& other) const
{
    return !(*this == other);
}

UniformBlock::UniformBlock(
        uint32_t binding,
        uint32_t size,
        VkShaderStageFlags stageFlags,
        UniformBlock::Type type)
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
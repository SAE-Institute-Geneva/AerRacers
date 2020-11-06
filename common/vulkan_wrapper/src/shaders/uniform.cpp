#include "vk/shaders/uniform.h"

namespace neko::vk
{
Uniform::Uniform(
        const std::uint32_t binding,
        const std::uint32_t offset,
        const std::uint32_t size,
        const std::uint32_t glType,
        const bool readOnly,
        const bool writeOnly,
        const VkShaderStageFlags stageFlags)
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
}
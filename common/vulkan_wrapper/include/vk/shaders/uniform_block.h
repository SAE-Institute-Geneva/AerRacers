#pragma once
#include <xxhash.h>
#include <map>

#include "vk/shaders/uniform.h"

namespace neko::vk
{
//Represents a block of multiple uniforms
struct UniformBlock
{
    enum class Type : std::uint8_t
    {
        UNIFORM = 0,
        STORAGE,
        PUSH
    };

    explicit UniformBlock(
            std::uint32_t binding = INVALID_INDEX,
            std::uint32_t size = INVALID_INDEX,
            VkShaderStageFlags stageFlags = 0,
            Type type = Type::UNIFORM);

    bool operator==(const UniformBlock& other) const;
    bool operator!=(const UniformBlock& other) const;

    [[nodiscard]] std::uint32_t GetBinding() const { return binding_; }
    [[nodiscard]] std::uint32_t GetSize() const { return size_; }
    [[nodiscard]] VkShaderStageFlags GetStageFlags() const { return stageFlags_; }
    [[nodiscard]] Type GetType() const { return type_; }

    void AddUniform(XXH64_hash_t uniformHash, const Uniform& uniform);
    [[nodiscard]] const Uniform& GetUniform(const XXH64_hash_t uniformHash) const
    { return uniforms_.at(uniformHash); }

private:
    std::uint32_t binding_ = INVALID_INDEX;
    std::uint32_t size_ = INVALID_INDEX;
    VkShaderStageFlags stageFlags_ = 0;
    Type type_ = Type::UNIFORM;

    std::map<XXH64_hash_t, Uniform> uniforms_{};
};
}
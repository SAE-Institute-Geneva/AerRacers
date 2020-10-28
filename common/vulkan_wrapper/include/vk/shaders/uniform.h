#pragma once
#include <xxh3.h>
#include <map>

#include "engine/globals.h"
#include "vk/vulkan_include.h"

namespace neko::vk
{
//Represents a single uniform variable in a shader
//If used without an UniformBlock, it represents a texture
struct Uniform
{
    explicit Uniform(
            uint32_t binding = INVALID_INDEX,
            uint32_t offset = INVALID_INDEX,
            uint32_t size = INVALID_INDEX,
            uint32_t glType = INVALID_INDEX,
            bool readOnly = false,
            bool writeOnly = false,
            VkShaderStageFlags stageFlags = 0);

    bool operator==(const Uniform& other) const;
    bool operator!=(const Uniform& other) const;

    [[nodiscard]] uint32_t GetBinding() const { return binding_; }
    [[nodiscard]] uint32_t GetOffset() const { return offset_; }
    [[nodiscard]] uint32_t GetSize() const { return size_; }
    [[nodiscard]] uint32_t GetGlType() const { return glType_; }
    [[nodiscard]] bool IsReadOnly() const { return readOnly_; }
    [[nodiscard]] bool IsWriteOnly() const { return writeOnly_; }
    [[nodiscard]] VkShaderStageFlags GetStageFlags() const { return stageFlags_; }

private:
    uint32_t binding_ = INVALID_INDEX;
    uint32_t offset_ = INVALID_INDEX;
    uint32_t size_ = INVALID_INDEX;
    uint32_t glType_ = INVALID_INDEX;
    bool readOnly_ = false;
    bool writeOnly_ = false;
    VkShaderStageFlags stageFlags_ = 0;
};

//Represents a block of multiple uniforms
struct UniformBlock
{
    enum class Type : uint8_t
    {
        UNIFORM = 0,
        STORAGE,
        PUSH
    };

    explicit UniformBlock(
            uint32_t binding = INVALID_INDEX,
            uint32_t size = INVALID_INDEX,
            VkShaderStageFlags stageFlags = 0,
            Type type = Type::UNIFORM);

    bool operator==(const UniformBlock& other) const;
    bool operator!=(const UniformBlock& other) const;

    [[nodiscard]] uint32_t GetBinding() const { return binding_; }
    [[nodiscard]] uint32_t GetSize() const { return size_; }
    [[nodiscard]] VkShaderStageFlags GetStageFlags() const { return stageFlags_; }
    [[nodiscard]] Type GetType() const { return type_; }

    void AddUniform(XXH64_hash_t uniformHash, const Uniform& uniform);
    [[nodiscard]] const Uniform& GetUniform(XXH64_hash_t uniformHash) const
    { return uniforms_.at(uniformHash); }

private:
    uint32_t binding_ = INVALID_INDEX;
    uint32_t size_ = INVALID_INDEX;
    VkShaderStageFlags stageFlags_ = 0;
    Type type_ = Type::UNIFORM;

    std::map<XXH64_hash_t, Uniform> uniforms_{};
};
}
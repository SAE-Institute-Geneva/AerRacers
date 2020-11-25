#pragma once
#include <map>
#include <xxhash.h>

#include "engine/globals.h"
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
		    std::string name = "",
            std::uint32_t binding = INVALID_INDEX,
            std::uint32_t size = INVALID_INDEX,
            VkShaderStageFlags stageFlags = 0,
            Type type = Type::UNIFORM);

	explicit UniformBlock(const json& uniformBlockJson);

    bool operator==(const UniformBlock& other) const;
    bool operator!=(const UniformBlock& other) const;

    [[nodiscard]] const std::string& GetName() const { return name_; }
    [[nodiscard]] std::uint32_t GetBinding() const { return binding_; }
    [[nodiscard]] std::uint32_t GetSize() const { return size_; }
    [[nodiscard]] VkShaderStageFlags GetStageFlags() const { return stageFlags_; }
    [[nodiscard]] Type GetType() const { return type_; }

    void AddUniform(const Uniform& uniform);
    [[nodiscard]] const Uniform& GetUniform(const XXH64_hash_t uniformHash) const
    { return uniforms_.at(uniformHash); }

	void FromJson(const json& uniformBlockJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
	std::string name_;
    std::uint32_t binding_ = INVALID_INDEX;
    std::uint32_t size_ = INVALID_INDEX;
    VkShaderStageFlags stageFlags_ = 0;
    Type type_ = Type::UNIFORM;

    std::map<XXH64_hash_t, Uniform> uniforms_{};
};
}

#pragma once
#include "engine/globals.h"
#include "vk/vulkan_include.h"

namespace neko::vk
{
//Represents a single uniform variable in a shader
//If used without an UniformBlock, it represents a texture
struct Uniform
{
	enum class Type : std::uint8_t
	{
		UNDEFINED = 0,
		SAMPLER_2D,
		SAMPLER_3D
	};

	//To use when used without an Uniform Block
    explicit Uniform(
			std::string name = "",
			std::uint32_t binding = INVALID_INDEX,
			Type type = Type::UNDEFINED,
			VkShaderStageFlags stageFlags = 0,
			bool readOnly = false,
			bool writeOnly = false);

    //To use when put inside an Uniform Block
    explicit Uniform(
		    std::string name = "",
		    std::uint32_t binding = INVALID_INDEX,
		    std::int32_t offset = -1,
		    std::int32_t size = -1,
		    VkShaderStageFlags stageFlags = 0,
		    bool readOnly = false,
		    bool writeOnly = false);


	explicit Uniform(const json& uniformJson);

    bool operator==(const Uniform& other) const;
    bool operator!=(const Uniform& other) const;

	[[nodiscard]] const std::string& GetName() const { return name_; }
    [[nodiscard]] std::uint32_t GetBinding() const { return binding_; }
    [[nodiscard]] std::uint32_t GetOffset() const { return offset_; }
    [[nodiscard]] std::uint32_t GetSize() const { return size_; }
    [[nodiscard]] Type GetType() const { return type_; }
    [[nodiscard]] bool IsReadOnly() const { return readOnly_; }
    [[nodiscard]] bool IsWriteOnly() const { return writeOnly_; }
    [[nodiscard]] VkShaderStageFlags GetStageFlags() const { return stageFlags_; }

	void FromJson(const json& uniformJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
	std::string name_;
    std::uint32_t binding_ = INVALID_INDEX;
    std::int32_t offset_ = -1;
    std::int32_t size_ = -1;
	Type type_ = Type::UNDEFINED;
	VkShaderStageFlags stageFlags_ = 0;
    bool readOnly_ = false;
    bool writeOnly_ = false;
};
}
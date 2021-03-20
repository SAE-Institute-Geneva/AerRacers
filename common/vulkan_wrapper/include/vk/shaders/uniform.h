#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "engine/globals.h"
#include "utils/json_utility.h"

#include "vk/vk_include.h"

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
	Uniform(std::string_view name     = "",
		std::uint32_t binding         = INVALID_INDEX,
		Type type                     = Type::UNDEFINED,
		VkShaderStageFlags stageFlags = 0,
		bool writeOnly                = false);

	//To use when put inside an Uniform Block
	Uniform(std::string_view name = "",
		std::uint32_t offset      = INVALID_INDEX,
		std::uint32_t size        = INVALID_INDEX,
		bool writeOnly            = false);

	explicit Uniform(const json& uniformJson);

	bool operator==(const Uniform& other) const;
	bool operator!=(const Uniform& other) const;

	[[nodiscard]] std::string_view GetName() const { return name_; }
	[[nodiscard]] std::uint32_t GetBinding() const { return binding_; }
	[[nodiscard]] std::uint32_t GetOffset() const { return offset_; }
	[[nodiscard]] std::uint32_t GetSize() const { return size_; }
	[[nodiscard]] Type GetType() const { return type_; }
	[[nodiscard]] VkShaderStageFlags GetStageFlags() const { return stageFlags_; }

	[[nodiscard]] bool IsWriteOnly() const { return writeOnly_; }

	void FromJson(const json& uniformJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
	std::string name_;
	std::uint32_t binding_         = INVALID_INDEX;
	std::uint32_t offset_          = INVALID_INDEX;
	std::uint32_t size_            = INVALID_INDEX;
	Type type_                     = Type::UNDEFINED;
	VkShaderStageFlags stageFlags_ = 0;
	bool writeOnly_                = false;
};
}    // namespace neko::vk
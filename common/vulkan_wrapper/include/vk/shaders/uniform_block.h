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

	UniformBlock(std::string_view name = "",
		std::uint32_t binding              = INVALID_INDEX,
		std::uint32_t size                 = INVALID_INDEX,
		VkShaderStageFlags stageFlags      = 0,
		Type type                          = Type::UNIFORM);

	explicit UniformBlock(const json& uniformBlockJson);

	bool operator==(const UniformBlock& other) const;
	bool operator!=(const UniformBlock& other) const;

	[[nodiscard]] std::string_view GetName() const { return name_; }
	[[nodiscard]] std::uint32_t GetBinding() const { return binding_; }
	[[nodiscard]] std::uint32_t GetSize() const { return size_; }
	[[nodiscard]] VkShaderStageFlags GetStageFlags() const { return stageFlags_; }
	[[nodiscard]] Type GetType() const { return type_; }
	[[nodiscard]] const Uniform& GetUniform(const StringHash uniformHash) const
	{
		return uniforms_.at(uniformHash);
	}

	void AddUniform(const Uniform& uniform);

	void FromJson(const json& uniformBlockJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
	std::string name_;
	std::uint32_t binding_         = INVALID_INDEX;
	std::uint32_t size_            = INVALID_INDEX;
	VkShaderStageFlags stageFlags_ = 0;
	Type type_                     = Type::UNIFORM;

	std::map<StringHash, Uniform> uniforms_ {};
};
}    // namespace neko::vk

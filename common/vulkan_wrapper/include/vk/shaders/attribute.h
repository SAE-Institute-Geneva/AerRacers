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
//Represents a shader attribute (e.g.: vertex positions, uv, normals...)
struct Attribute
{
	enum class Type
	{
		UNDEFINED = 0,

		// Float Types
		FLOAT, VEC2F, VEC3F, VEC4F,

		// Int Types
		INT, VEC2I, VEC3I, VEC4I,

		// Unsigned Types
		UINT, VEC2U, VEC3U, VEC4U,

		// Matrix Types
		MAT2, MAT3, MAT4
	};

	Attribute(std::string_view name = "",
		std::uint32_t location = INVALID_INDEX,
		std::uint32_t size     = INVALID_INDEX,
		Type type              = Type::UNDEFINED);

	explicit Attribute(const json& attributeJson) { FromJson(attributeJson); }

	[[nodiscard]] std::string_view GetName() const { return name_; }
	[[nodiscard]] std::uint32_t GetLocation() const { return location_; }
	[[nodiscard]] std::uint32_t GetSize() const { return size_; }
	[[nodiscard]] Type GetType() const { return type_; }

	[[nodiscard]] constexpr VkFormat GetVkFormat() const;

	void FromJson(const json& attributeJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
	std::string name_;
	std::uint32_t location_ = INVALID_INDEX;
	std::uint32_t size_     = INVALID_INDEX;
	Type type_              = Type::UNDEFINED;
};
}    // namespace neko::vk

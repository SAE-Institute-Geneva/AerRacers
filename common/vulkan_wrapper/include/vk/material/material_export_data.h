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
#include <variant>

#include "graphics/color.h"

#include "vk/images/image2d.h"

namespace neko::vk
{
//Used for exporting data to the material's shader
class MaterialExportData
{
public:
	enum Type : std::uint8_t
	{
		FLOAT = 0,
		INT,
		UCHAR,
		IMAGE_2D,
		VEC2,
		COLOR
	};

	MaterialExportData() = default;
	explicit MaterialExportData(float f) : data_(f) {}
	explicit MaterialExportData(int i) : data_(i) {}
	explicit MaterialExportData(std::uint8_t i) : data_(i) {}
	explicit MaterialExportData(const Image2d* image2d) : data_(image2d) {}
	explicit MaterialExportData(Vec2f vec2) : data_(vec2) {}
	explicit MaterialExportData(Color4 color) : data_(color) {}

	MaterialExportData(const MaterialExportData& other) = default;

	[[nodiscard]] float GetFloat() const { return std::get<float>(data_); }
	[[nodiscard]] int GetInt() const { return std::get<int>(data_); }
	[[nodiscard]] int GetUChar() const { return std::get<std::uint8_t>(data_); }
	[[nodiscard]] const Image2d* GetImage2d() const { return std::get<const Image2d*>(data_); }
	[[nodiscard]] Vec2f GetVec2() const { return std::get<Vec2f>(data_); }
	[[nodiscard]] Color4 GetColor() const { return std::get<Color4>(data_); }

	[[nodiscard]] Type GetType() const { return static_cast<Type>(data_.index()); }

private:
	std::variant<float, int, std::uint8_t, const Image2d*, Vec2f, Color4> data_;
};
}    // namespace neko::vk

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
		IMAGE_2D,
		//IMAGE_CUBE,
		COLOR
	};

	explicit MaterialExportData(float f) : type_(Type::FLOAT), float_(f) {}

	explicit MaterialExportData(int i) : type_(Type::INT), integer_(i) {}

	explicit MaterialExportData(const Image2d* image2d) : type_(Type::IMAGE_2D), image2d_(image2d)
	{}

	//explicit MaterialExportData(const ImageCube* imageCube)
	//    : type_(Type::IMAGE_CUBE),
	//      imageCube_(imageCube) { }

	explicit MaterialExportData(Color4 color) : type_(Type::COLOR), color_(color) {}

	[[nodiscard]] float GetFloat() const { return float_; }
	[[nodiscard]] int GetInt() const { return integer_; }
	[[nodiscard]] const Image2d* GetImage2d() const { return image2d_; }
	//[[nodiscard]] const ImageCube* GetImageCube() const { return imageCube_; }
	[[nodiscard]] Color4 GetColor() const { return color_; }

	[[nodiscard]] Type GetType() const { return type_; }

private:
	Type type_;

	union
	{
		float float_ = 0.0f;
		int integer_;
		const Image2d* image2d_;
		//const ImageCube* imageCube_;
		Color4 color_;
	};
};
}    // namespace neko::vk

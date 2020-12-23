#pragma once
#include "graphics/color.h"
#include "vk/images/image2d.h"

namespace neko::vk
{
//Used for exporting data to the material's shader
class MaterialExportData
{
public:
	enum class Type : std::uint8_t
	{
		FLOAT = 0,
		INT,
		IMAGE_2D,
		//IMAGE_CUBE,
		COLOR
	};

	explicit MaterialExportData(float f)
			: type_(Type::FLOAT),
			  float_(f) {}

	explicit MaterialExportData(int i)
			: type_(Type::INT),
			  integer_(i) {}

	explicit MaterialExportData(const Image2d* image2d)
			: type_(Type::IMAGE_2D),
			  image2d_(image2d) {}

	//explicit MaterialExportData(const ImageCube* imageCube)
	//    : type_(Type::IMAGE_CUBE),
	//      imageCube_(imageCube) { }

	explicit MaterialExportData(Color4 color)
			: type_(Type::COLOR),
			  color_(color) {}

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
}

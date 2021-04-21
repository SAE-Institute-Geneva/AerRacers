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
#include "vk/material/material.h"

namespace neko::vk
{
constexpr std::string_view kColorUiName     = "color";
constexpr std::string_view kSlidingCropName = "slidingCrop";
constexpr std::string_view kTextureName     = "tex";
constexpr StringHash kColorUiHash           = HashString(kColorUiName);
constexpr StringHash kSlidingCropHash       = HashString(kSlidingCropName);
constexpr StringHash kTextureHash           = HashString(kTextureName);

class UiMaterial : public Material
{
public:
	enum UiType : std::uint8_t
	{
		IMAGE = 0,
		TEXT
	};

	UiMaterial(std::string_view name                    = "",
		Color4 color                                    = Color::white,
		const std::optional_const_ref<Image2d>& texture = std::nullopt);

	bool operator==(const UiMaterial& other) const;
	bool operator!=(const UiMaterial& other) const { return !(*this == other); }

	[[nodiscard]] std::string GetShaderPath() const override;
	[[nodiscard]] MaterialType GetType() const override { return MaterialType::UI; }

	void SetColor(const Color4& color);
	[[nodiscard]] Color4 GetColor() const { return color_; }

	void SetSlidingCrop(Vec2f slidingCrop);
	[[nodiscard]] Vec2f GetSlidingCrop() const { return slidingCrop_; }

	void SetTexture(const Image2d& texture);
	[[nodiscard]] std::optional_const_ref<Image2d> GetTexture() const { return texture_; }
	void ResetTexture();

	void SetRenderMode(RenderMode renderMode) override {}
	[[nodiscard]] RenderMode GetRenderMode() const override { return RenderMode::VK_TRANSPARENT; }

	void CreatePipeline(const VertexInput& vertexInput) override;

	[[nodiscard]] ordered_json ToJson() const override;
	void FromJson(const json& materialJson) override;

private:
	void ResetPipeline();

	UiType type_ = IMAGE;

	Color4 color_                             = Color::white;
	Vec2f slidingCrop_                        = Vec2f::one;
	std::optional_const_ref<Image2d> texture_ = std::nullopt;
};
}    // namespace neko::vk

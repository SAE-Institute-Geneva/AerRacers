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
constexpr std::string_view kDiffuseName  = ("diffuseMap");
constexpr std::string_view kSpecularName = ("specularMap");
constexpr std::string_view kNormalName   = ("normalMap");
constexpr std::string_view kColorName    = ("color");
constexpr std::string_view kShininessName = ("shininess");
constexpr StringHash kDiffuseHash        = HashString(kDiffuseName);
constexpr StringHash kSpecularHash       = HashString(kSpecularName);
constexpr StringHash kNormalHash         = HashString(kNormalName);
constexpr StringHash kColorHash          = HashString(kColorName);
constexpr StringHash kShininessHash       = HashString(kShininessName);

class DiffuseMaterial : public Material
{
public:
	enum TextureMaps : std::uint8_t
	{
		DIFFUSE  = 1u << 0u,
		SPECULAR = 1u << 1u,
		NORMAL   = 1u << 2u,
		EMISSIVE = 1u << 3u,
	};

	DiffuseMaterial(std::string_view name                       = "",
		Color4 color                                            = Color::white,
		const std::optional_const_ref<Image2d>& textureAlbedo   = std::nullopt,
		const std::optional_const_ref<Image2d>& textureSpecular = std::nullopt,
		const std::optional_const_ref<Image2d>& textureNormal   = std::nullopt);

	bool operator==(const DiffuseMaterial& other) const;
	bool operator!=(const DiffuseMaterial& other) const { return !(*this == other); }

	[[nodiscard]] std::string GetShaderPath() const override;
	[[nodiscard]] MaterialType GetType() const override { return MaterialType::DIFFUSE; }

	void SetColor(const Color4& color);
	[[nodiscard]] Color4 GetColor() const { return color_; }

	void SetDiffuse(const Image2d& textureDiffuse);
	[[nodiscard]] std::optional_const_ref<Image2d> GetDiffuse() const { return diffuse_; }
	void ResetDiffuse();

	void SetSpecular(const Image2d& textureSpecular);
	[[nodiscard]] std::optional_const_ref<Image2d> GetSpecular() const { return specular_; }
	void ResetSpecular();

	void SetNormal(const Image2d& textureNormal);
	[[nodiscard]] std::optional_const_ref<Image2d> GetNormal() const { return normal_; }
	void ResetNormal();

	void SetShininess(const float shininess);
	[[nodiscard]] float GetShininess() const { return shininess_; }

	void SetRenderMode(RenderMode renderMode) override;
	[[nodiscard]] RenderMode GetRenderMode() const override { return renderMode_; }

	void CreatePipeline(const VertexInput& vertexInput) override;

	[[nodiscard]] ordered_json ToJson() const override;
	void FromJson(const json& materialJson) override;

private:
	void ResetPipeline();

	Color4 color_;
	std::optional_const_ref<Image2d> diffuse_;
	std::optional_const_ref<Image2d> specular_;
	std::optional_const_ref<Image2d> normal_;

	float shininess_ = 32.0f;
};
}    // namespace neko::vk

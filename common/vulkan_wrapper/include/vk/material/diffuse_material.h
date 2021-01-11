#pragma once
#include "mathematics/hash.h"
#include "utils/json_utility.h"
#include "vk/material/material.h"

namespace neko::vk
{
class DiffuseMaterial : public Material
{
public:
	explicit DiffuseMaterial(
			const std::string& name = "",
			Color4 color = Color4(Color::white, 1.0f),
			const std::neko::optional<const Image2d&>& textureAlbedo = std::neko::nullopt,
			const std::neko::optional<const Image2d&>& textureSpecular = std::neko::nullopt,
			const std::neko::optional<const Image2d&>& textureNormal = std::neko::nullopt);

	bool operator==(const DiffuseMaterial& other) const;
	bool operator!=(const DiffuseMaterial& other) const { return !(*this == other); }

	void CreatePipeline(const VertexInput& vertexInput) override;

	void SetShaderPath(const std::string& shaderPath) { shaderPath_ = shaderPath; }
	[[nodiscard]] std::string GetShaderPath() const { return shaderPath_; }
	[[nodiscard]] MaterialType GetType() const override { return MaterialType::DIFFUSE; }

	void SetColor(const Color4& color);
	[[nodiscard]] Color4 GetColor() const { return color_; }

	void SetDiffuse(const Image2d& textureDiffuse);
	[[nodiscard]] std::neko::optional<const Image2d&> GetDiffuse() const { return diffuse_; }
	void ResetDiffuse();

	void SetSpecular(const Image2d& textureSpecular);
	[[nodiscard]] std::neko::optional<const Image2d&> GetSpecular() const { return specular_; }
	void ResetSpecular();

	void SetNormal(const Image2d& textureNormal);
	[[nodiscard]] std::neko::optional<const Image2d&> GetNormal() const { return normal_; }
	void ResetNormal();

	void SetRenderMode(RenderMode renderMode) override;
	[[nodiscard]] RenderMode GetRenderMode() const override { return renderMode_; }

	[[nodiscard]] ordered_json ToJson() const override;
	void FromJson(const json& materialJson) override;

private:
	void ResetPipeline();

	std::string shaderPath_;

	Color4 color_;
	std::neko::optional<const Image2d&> diffuse_;
	std::neko::optional<const Image2d&> specular_;
	std::neko::optional<const Image2d&> normal_;

	inline static const StringHash kDiffuseHash = HashString("diffuse");
	inline static const StringHash kSpecularHash = HashString("specular");
	inline static const StringHash kNormalHash = HashString("normal");
	inline static const StringHash kColorHash = HashString("color");
};
}

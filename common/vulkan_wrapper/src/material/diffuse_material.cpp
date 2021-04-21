#include "vk/material/diffuse_material.h"

#include "engine/engine.h"
#include "engine/resource_locations.h"

#include "vk/models/mesh_instance.h"

namespace neko::vk
{
DiffuseMaterial::DiffuseMaterial(std::string_view name,
	Color4 color,
	const std::optional_const_ref<Image2d>& textureAlbedo,
	const std::optional_const_ref<Image2d>& textureSpecular,
	const std::optional_const_ref<Image2d>& textureNormal)
   : Material(name),
	 color_(color),
	 diffuse_(textureAlbedo),
	 specular_(textureSpecular),
	 normal_(textureNormal)
{
	const auto baseDiffuse = MaterialExportData(color_);
	uniformData_.emplace(kColorHash, baseDiffuse);

	if (diffuse_)
	{
		const auto diffuse = MaterialExportData(&diffuse_->get());
		descriptorData_.emplace(kDiffuseHash, diffuse);
	}

	if (specular_)
	{
		const auto mra = MaterialExportData(&specular_->get());
		descriptorData_.emplace(kSpecularHash, mra);
	}

	if (normal_)
	{
		const auto normal = MaterialExportData(&normal_->get());
		descriptorData_.emplace(kNormalHash, normal);
	}

	const auto shininess = MaterialExportData(shininess_);
	uniformData_.emplace(kShininessHash, shininess);
}

bool DiffuseMaterial::operator==(const DiffuseMaterial& other) const
{
	return /*shaderPath_ == other.shaderPath_ && */ color_ == other.color_ &&
	       ((diffuse_ && other.diffuse_ && &diffuse_.value() == &other.diffuse_.value()) ||
			   (!diffuse_ && !other.diffuse_)) &&
	       ((specular_ && other.specular_ && &specular_.value() == &other.specular_.value()) ||
			   (!specular_ && !other.specular_)) &&
	       ((normal_ && other.normal_ && &normal_.value() == &other.normal_.value()) ||
			   (!normal_ && !other.normal_));
}

void DiffuseMaterial::CreatePipeline(const VertexInput&)
{
	if (pipelineMaterial_) return;

	ResetPipeline();
}

std::string DiffuseMaterial::GetShaderPath() const
{
	std::string shaderName = "lights";
	if (diffuse_) shaderName += "_d";
	if (specular_) shaderName += "_s";
	if (normal_) shaderName = "_n";

	return GetVkShadersFolderPath() + shaderName + ".aershader";
}

void DiffuseMaterial::SetColor(const Color4& color)
{
	color_ = color;

	auto colorData = MaterialExportData(color_);
	for (auto& data : uniformData_)
	{
		if (data.first == kColorHash)
		{
			data.second = colorData;
			return;
		}
	}

	uniformData_.emplace(kColorHash, colorData);
}

void DiffuseMaterial::SetDiffuse(const Image2d& textureDiffuse)
{
	diffuse_ = std::optional_const_ref<Image2d>(textureDiffuse);

	auto diffuse = MaterialExportData(&diffuse_->get());
	const auto it = descriptorData_.find(kDiffuseHash);
	if (it != descriptorData_.end())
	{
		descriptorData_[kDiffuseHash] = diffuse;
		return;
	}

	descriptorData_.emplace(kDiffuseHash, diffuse);
	ResetPipeline();
}

void DiffuseMaterial::ResetDiffuse()
{
	diffuse_.reset();

	const auto it = descriptorData_.find(kDiffuseHash);
	if (it != descriptorData_.end()) descriptorData_.erase(it);

	ResetPipeline();
}

void DiffuseMaterial::SetSpecular(const Image2d& textureSpecular)
{
	specular_ = std::optional_const_ref<Image2d>(textureSpecular);

	auto specular = MaterialExportData(&specular_->get());
	const auto it = descriptorData_.find(kSpecularHash);
	if (it != descriptorData_.end())
	{
		descriptorData_[kSpecularHash] = specular;
		return;
	}

	descriptorData_.emplace(kSpecularHash, specular);
	ResetPipeline();
}

void DiffuseMaterial::ResetSpecular()
{
	specular_.reset();

	const auto it = descriptorData_.find(kSpecularHash);
	if (it != descriptorData_.end()) descriptorData_.erase(it);

	ResetPipeline();
}

void DiffuseMaterial::SetNormal(const Image2d& textureNormal)
{
	normal_ = std::optional_const_ref<Image2d>(textureNormal);

	auto normal = MaterialExportData(&normal_->get());
	const auto it = descriptorData_.find(kNormalHash);
	if (it != descriptorData_.end())
	{
		descriptorData_[kNormalHash] = normal;
		return;
	}

	descriptorData_.emplace(kNormalHash, normal);
	ResetPipeline();
}

void DiffuseMaterial::ResetNormal()
{
	normal_.reset();

	const auto it = descriptorData_.find(kNormalHash);
	if (it != descriptorData_.end()) descriptorData_.erase(it);

	ResetPipeline();
}

void DiffuseMaterial::SetShininess(const float shininess)
{
	shininess_ = shininess;
	descriptorData_[kShininessHash] = MaterialExportData(shininess);
}

void DiffuseMaterial::SetRenderMode(const Material::RenderMode renderMode)
{
	renderMode_ = renderMode;
	ResetPipeline();
}

void DiffuseMaterial::ResetPipeline()
{
	const VkCullModeFlags cullMode = renderMode_ == RenderMode::VK_OPAQUE ?
                                         VK_CULL_MODE_FRONT_BIT :
                                         static_cast<VkCullModeFlags>(0);

	const PipelineStage stage =
		renderMode_ == RenderMode::VK_OPAQUE ? PipelineStage {0, 0} : PipelineStage {0, 1};

	const Configuration& config = BasicEngine::GetInstance()->GetConfig();
	pipelineMaterial_ =
		std::optional_ref<MaterialPipeline>(MaterialPipeline::CreateMaterialPipeline(stage,
			GraphicsPipelineCreateInfo(GetShaderPath(),
				{Vertex::GetVertexInput(0), ModelInstance::Instance::GetVertexInput(1)},
				GraphicsPipeline::Mode::MRT,
				GraphicsPipeline::Depth::READ_WRITE,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_POLYGON_MODE_FILL,
				cullMode,
				VK_FRONT_FACE_CLOCKWISE)));
}

ordered_json DiffuseMaterial::ToJson() const
{
	ordered_json materialJson;
	materialJson["name"]       = name_;
	materialJson["type"]       = GetType();
	materialJson["color"]["r"] = color_.r;
	materialJson["color"]["g"] = color_.g;
	materialJson["color"]["b"] = color_.b;
	materialJson["color"]["a"] = color_.a;

	if (diffuse_) materialJson["diffusePath"] = diffuse_->get().GetFilePath();
	if (specular_) materialJson["specularPath"] = specular_->get().GetFilePath();
	if (normal_) materialJson["normalPath"] = normal_->get().GetFilePath();

	return materialJson;
}

void DiffuseMaterial::FromJson(const json& materialJson)
{
	name_       = materialJson["name"].get<std::string_view>();

	Color4 color;
	color.r = materialJson["color"]["r"].get<float>();
	color.g = materialJson["color"]["g"].get<float>();
	color.b = materialJson["color"]["b"].get<float>();
	color.a = materialJson["color"]["a"].get<float>();
	SetColor(color);

	auto& textureManager = TextureManagerLocator::get();
	if (CheckJsonExists(materialJson, "diffusePath"))
	{
		const auto& path = materialJson["diffusePath"].get<std::string>();
		SetDiffuse(*textureManager.GetTexture(path));
	}

	if (CheckJsonExists(materialJson, "specularPath"))
	{
		const auto& path = materialJson["specularPath"].get<std::string>();
		SetSpecular(*textureManager.GetTexture(path));
	}

	if (CheckJsonExists(materialJson, "normalPath"))
	{
		const auto& path = materialJson["normalPath"].get<std::string>();
		SetNormal(*textureManager.GetTexture(path));
	}
}
}    // namespace neko::vk
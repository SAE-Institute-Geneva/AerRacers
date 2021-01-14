#include "vk/material/diffuse_material.h"

#include "engine/engine.h"
#include "vk/models/mesh_instance.h"

namespace neko::vk
{
DiffuseMaterial::DiffuseMaterial(
		const std::string& name,
		const Color4 color,
		const std::neko::optional<const Image2d&>& textureAlbedo,
		const std::neko::optional<const Image2d&>& textureSpecular,
		const std::neko::optional<const Image2d&>& textureNormal)
		: Material(name),
		color_(color),
		diffuse_(textureAlbedo),
		specular_(textureSpecular),
		normal_(textureNormal)
{
	const auto baseDiffuse = MaterialExportData(color_);
	uniformData_.emplace_back(kColorHash, baseDiffuse);

	if (diffuse_)
	{
		const auto diffuse = MaterialExportData(&*diffuse_);
		descriptorData_.emplace_back(kDiffuseHash, diffuse);
	}

	if (specular_)
	{
		const auto mra = MaterialExportData(&*specular_);
		descriptorData_.emplace_back(kSpecularHash, mra);
	}

	if (normal_)
	{
		const auto normal = MaterialExportData(&*normal_);
		descriptorData_.emplace_back(kNormalHash, normal);
	}
}

bool DiffuseMaterial::operator==(const DiffuseMaterial& other) const
{
	return shaderPath_ == other.shaderPath_
	       && color_ == other.color_
	       && ((diffuse_ && other.diffuse_ && &diffuse_.value() == &other.diffuse_.value()) || (!diffuse_ && !other.diffuse_))
	       && ((specular_ && other.specular_ && &specular_.value() == &other.specular_.value()) || (!specular_ && !other.specular_))
	       && ((normal_ && other.normal_ && &normal_.value() == &other.normal_.value()) || (!normal_ && !other.normal_));
}

void DiffuseMaterial::CreatePipeline(const VertexInput& vertexInput)
{
	if (pipelineMaterial_)
		return;

	ResetPipeline();
}

void DiffuseMaterial::SetColor(const Color4& color)
{
	color_ = color;

	auto baseDiffuseData = MaterialExportData(color_);
	for (auto& data : uniformData_)
	{
		if (data.first == kColorHash)
		{
			data.second = baseDiffuseData;
			return;
		}
	}

	uniformData_.emplace_back(kColorHash, baseDiffuseData);
}

void DiffuseMaterial::SetDiffuse(const Image2d& textureDiffuse)
{
	diffuse_ = std::neko::optional<const Image2d&>(textureDiffuse);

	auto diffuse = MaterialExportData(&*diffuse_);
	for (auto& data : descriptorData_)
	{
		if (data.first == kDiffuseHash)
		{
			data.second = diffuse;
			return;
		}
	}

	descriptorData_.emplace_back(kDiffuseHash, diffuse);
	ResetPipeline();
}

void DiffuseMaterial::ResetDiffuse()
{
	diffuse_.reset();

	for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it)
	{
		if (it->first == kDiffuseHash)
		{
			descriptorData_.erase(it);
			break;
		}
	}

	ResetPipeline();
}

void DiffuseMaterial::SetSpecular(const Image2d& textureSpecular)
{
	specular_ = std::neko::optional<const Image2d&>(textureSpecular);

	auto specular = MaterialExportData(&*specular_);
	for (auto& data : descriptorData_)
	{
		if (data.first == kSpecularHash)
		{
			data.second = specular;
			return;
		}
	}

	descriptorData_.emplace_back(kSpecularHash, specular);
	ResetPipeline();
}

void DiffuseMaterial::ResetSpecular()
{
	specular_.reset();

	for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it)
	{
		if (it->first == kSpecularHash)
		{
			descriptorData_.erase(it);
			break;
		}
	}

	ResetPipeline();
}

void DiffuseMaterial::SetNormal(const Image2d& textureNormal)
{
	normal_ = std::neko::optional<const Image2d&>(textureNormal);

	auto normal = MaterialExportData(&*normal_);
	for (auto& data : descriptorData_)
	{
		if (data.first == kNormalHash)
		{
			data.second = normal;
			return;
		}
	}

	descriptorData_.emplace_back(kNormalHash, normal);
	ResetPipeline();
}

void DiffuseMaterial::ResetNormal()
{
	normal_.reset();

	for (auto it = descriptorData_.begin(); it != descriptorData_.end(); ++it)
	{
		if (it->first == kNormalHash)
		{
			descriptorData_.erase(it);
			break;
		}
	}

	ResetPipeline();
}

void DiffuseMaterial::SetRenderMode(const Material::RenderMode renderMode)
{
	renderMode_ = renderMode;
	ResetPipeline();
}

void DiffuseMaterial::ResetPipeline()
{
	const VkCullModeFlags cullMode =
			renderMode_ == RenderMode::VK_OPAQUE ?
			VK_CULL_MODE_FRONT_BIT :
			static_cast<VkCullModeFlags>(0);

	const Pipeline::Stage stage =
			renderMode_ == RenderMode::VK_OPAQUE ?
			Pipeline::Stage{0, 0} : Pipeline::Stage{0, 1};

	const auto& config = BasicEngine::GetInstance()->GetConfig();
	pipelineMaterial_ = std::neko::optional<MaterialPipeline&>(
			MaterialPipeline::CreateMaterialPipeline(
					stage,
					GraphicsPipelineCreateInfo(
							config.dataRootPath + shaderPath_,
							{
									Vertex::GetVertexInput(0),
									MeshInstance::Instance::GetVertexInput(1)
							},
							GraphicsPipeline::Mode::MRT,
							GraphicsPipeline::Depth::READ_WRITE,
							VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
							VK_POLYGON_MODE_FILL,
							cullMode,
							VK_FRONT_FACE_CLOCKWISE
					)));
}

ordered_json DiffuseMaterial::ToJson() const
{
	ordered_json materialJson;
	materialJson["name"] = name_;
	materialJson["shaderPath"] = shaderPath_;
	materialJson["type"] = GetType();
	materialJson["color"]["r"] = color_.r;
	materialJson["color"]["g"] = color_.g;
	materialJson["color"]["b"] = color_.b;
	materialJson["color"]["a"] = color_.a;

	if (diffuse_)
	{
		materialJson["diffusePath"] = diffuse_->GetFilePath();
	}

	if (specular_)
	{
		materialJson["specularPath"] = specular_->GetFilePath();
	}

	if (normal_)
	{
		materialJson["normalPath"] = normal_->GetFilePath();
	}

	return materialJson;
}

void DiffuseMaterial::FromJson(const json& materialJson)
{
	name_ = materialJson["name"].get<std::string>();
	shaderPath_ = materialJson["shaderPath"].get<std::string>();

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
		SetDiffuse(textureManager.GetImage2d(path));
	}

	if (CheckJsonExists(materialJson, "specularPath"))
	{
		const auto& path = materialJson["specularPath"].get<std::string>();
		SetDiffuse(textureManager.GetImage2d(path));
	}

	if (CheckJsonExists(materialJson, "normalPath"))
	{
		const auto& path = materialJson["normalPath"].get<std::string>();
		SetDiffuse(textureManager.GetImage2d(path));
	}
}
}
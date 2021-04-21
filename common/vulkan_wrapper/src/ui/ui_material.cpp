#include "vk/ui/ui_material.h"

#include "engine/resource_locations.h"

#include "vk/models/mesh_instance.h"

namespace neko::vk
{
UiMaterial::UiMaterial(
	std::string_view name, Color4 color, const std::optional_const_ref<Image2d>& texture)
   : Material(name), color_(color), texture_(texture)
{
	const auto objColor = MaterialExportData(color_);
	uniformData_.emplace(kColorUiHash, objColor);

	const auto objCrop = MaterialExportData(slidingCrop_);
	uniformData_.emplace(kSlidingCropHash, objCrop);

	if (texture_)
	{
		const auto texture = MaterialExportData(&texture_->get());
		descriptorData_.emplace(kDiffuseHash, texture);
	}
}

bool UiMaterial::operator==(const UiMaterial& other) const
{
	return color_ == other.color_;
}

void UiMaterial::CreatePipeline(const VertexInput&)
{
	if (pipelineMaterial_) return;

	ResetPipeline();
}

std::string UiMaterial::GetShaderPath() const
{
	std::string shaderName;
	switch (type_)
	{
		case IMAGE: shaderName = "ui_image"; break;
		case TEXT: shaderName = "ui_text"; break;
	}

	return GetVkShadersFolderPath() + "ui/" + shaderName + ".aershader";
}

void UiMaterial::SetColor(const Color4& color)
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

void UiMaterial::SetSlidingCrop(Vec2f slidingCrop)
{
	slidingCrop_ = slidingCrop;

	auto cropData = MaterialExportData(slidingCrop_);
	for (auto& data : uniformData_)
	{
		if (data.first == kSlidingCropHash)
		{
			data.second = cropData;
			return;
		}
	}

	uniformData_.emplace(kSlidingCropHash, cropData);
}

void UiMaterial::SetTexture(const Image2d& texture)
{
	texture_ = std::optional_const_ref<Image2d>(texture);

	const auto tex = MaterialExportData(&texture_->get());
	const auto it  = descriptorData_.find(kTextureHash);
	if (it != descriptorData_.end())
	{
		descriptorData_[kTextureHash] = tex;
		return;
	}

	descriptorData_.emplace(kTextureHash, tex);
	ResetPipeline();
}

void UiMaterial::ResetTexture()
{
	texture_.reset();

	const auto it = descriptorData_.find(kTextureHash);
	if (it != descriptorData_.end()) descriptorData_.erase(it);

	ResetPipeline();
}

void UiMaterial::ResetPipeline()
{
	const VkCullModeFlags cullMode = VK_CULL_MODE_FRONT_BIT;
	const PipelineStage stage      = PipelineStage {0, 1};
	pipelineMaterial_ =
		std::optional_ref<MaterialPipeline>(MaterialPipeline::CreateMaterialPipeline(stage,
			GraphicsPipelineCreateInfo(GetShaderPath(),
				{RenderQuadUi::GetVertexInput(0)},
				GraphicsPipeline::Mode::MRT,
				GraphicsPipeline::Depth::NONE,
				VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
				VK_POLYGON_MODE_FILL,
				cullMode,
				VK_FRONT_FACE_CLOCKWISE)));
}

ordered_json UiMaterial::ToJson() const
{
	ordered_json materialJson;
	materialJson["name"]             = name_;
	materialJson["type"]             = GetType();
	materialJson["color"]["r"]       = color_.r;
	materialJson["color"]["g"]       = color_.g;
	materialJson["color"]["b"]       = color_.b;
	materialJson["color"]["a"]       = color_.a;
	materialJson["slidingCrop"]["x"] = slidingCrop_.x;
	materialJson["slidingCrop"]["y"] = slidingCrop_.y;

	if (texture_) materialJson["texturePath"] = texture_->get().GetFilePath();

	return materialJson;
}

void UiMaterial::FromJson(const json& materialJson)
{
	name_ = materialJson["name"].get<std::string_view>();

	Color4 color;
	color.r = materialJson["color"]["r"].get<float>();
	color.g = materialJson["color"]["g"].get<float>();
	color.b = materialJson["color"]["b"].get<float>();
	color.a = materialJson["color"]["a"].get<float>();
	SetColor(color);

	Vec2f slidingCrop;
	slidingCrop.x = materialJson["slidingCrop"]["x"].get<float>();
	slidingCrop.y = materialJson["slidingCrop"]["y"].get<float>();
	SetSlidingCrop(slidingCrop);

	auto& textureManager = TextureManagerLocator::get();
	if (CheckJsonExists(materialJson, "texturePath"))
	{
		const auto& path = materialJson["texturePath"].get<std::string>();
		SetTexture(*textureManager.GetTexture(path));
	}
}
}    // namespace neko::vk
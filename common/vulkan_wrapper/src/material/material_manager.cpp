#include "vk/material/material_manager.h"

namespace neko::vk
{
MaterialManager::MaterialManager()
{
	diffuseMaterials_.reserve(kDefaultMaterialNum);
	uiMaterials_.reserve(kDefaultUiMaterialNum);

	MaterialManagerLocator::provide(this);
}

void MaterialManager::Clear()
{
	diffuseMaterials_.clear();
	uiMaterials_.clear();

	//skyboxMaterialIds_.clear();
	//skyboxMaterials_.clear();

	//trailMaterialIds_.clear();
	//trailMaterials_.clear();

	//particleMaterials_.clear();
	//particleMaterialIds_.clear();

	diffuseMaterials_.emplace(kDefaultDiffuseMaterialId, DiffuseMaterial());
	diffuseMaterials_[kDefaultDiffuseMaterialId].CreatePipeline(Vertex::GetVertexInput(0));
}

ResourceHash MaterialManager::AddNewMaterial(std::string_view name, MaterialType materialType)
{
	const ResourceHash resourceId = HashString(name);
	switch (materialType)
	{
		case MaterialType::DIFFUSE:
		{
			if (diffuseMaterials_.find(resourceId) != diffuseMaterials_.cend()) return resourceId;
			diffuseMaterials_.emplace(resourceId, DiffuseMaterial());
			break;
		}
		case MaterialType::UI:
		{
			if (uiMaterials_.find(resourceId) != uiMaterials_.cend()) return resourceId;
			uiMaterials_.emplace(resourceId, UiMaterial());
			break;
		}
	}

	return resourceId;
}

Material& MaterialManager::GetMaterial(std::string_view materialName)
{
	return GetMaterial(HashString(materialName));
}

Material& MaterialManager::GetMaterial(const ResourceHash resourceId)
{
	if (resourceId == kDefaultDiffuseMaterialId &&
		diffuseMaterials_.find(resourceId) == diffuseMaterials_.cend())
	{
		diffuseMaterials_.emplace(kDefaultDiffuseMaterialId, DiffuseMaterial());
		diffuseMaterials_[kDefaultDiffuseMaterialId].CreatePipeline(Vertex::GetVertexInput(0));
	}

	// Diffuse materials
	if (diffuseMaterials_.find(resourceId) != diffuseMaterials_.cend())
		return diffuseMaterials_[resourceId];

	// Ui materials
	if (uiMaterials_.find(resourceId) != uiMaterials_.cend())
		return uiMaterials_[resourceId];

	//Particles materials
	//for (size_t i = 0; i < particleMaterialIDs_.size(); i++)
	//	if (particleMaterialIDs_[i] == resourceID) return particleMaterials_[i];

	//Trails materials
	//for (size_t i = 0; i < trailMaterialIDs_.size(); i++)
	//	if (trailMaterialIDs_[i] == resourceID) return trailMaterials_[i];

	//Skybox materials
	//for (size_t i = 0; i < skyboxMaterialIDs_.size(); i++)
	//	if (skyboxMaterialIDs_[i] == resourceID) return skyboxMaterials_[i];

	logDebug(fmt::format("The material with ID {} isn't loaded!", resourceId));
	return GetDiffuseMaterial(kDefaultDiffuseMaterialId);
}

DiffuseMaterial& MaterialManager::GetDiffuseMaterial(std::string_view materialName)
{
	return GetDiffuseMaterial(HashString(materialName));
}

DiffuseMaterial& MaterialManager::GetDiffuseMaterial(ResourceHash resourceId)
{
	if (resourceId == kDefaultDiffuseMaterialId &&
	    diffuseMaterials_.find(resourceId) == diffuseMaterials_.cend())
	{
		diffuseMaterials_.emplace(kDefaultDiffuseMaterialId, DiffuseMaterial());
		diffuseMaterials_[kDefaultDiffuseMaterialId].CreatePipeline(Vertex::GetVertexInput(0));
	}

	return diffuseMaterials_[resourceId];
}

UiMaterial& MaterialManager::GetUiMaterial(std::string_view materialName)
{
	return GetUiMaterial(HashString(materialName));
}

UiMaterial& MaterialManager::GetUiMaterial(ResourceHash resourceId)
{
	return uiMaterials_[resourceId];
}

bool MaterialManager::IsMaterialLoaded(std::string_view materialName)
{
	return IsMaterialLoaded(HashString(materialName));
}

bool MaterialManager::IsMaterialLoaded(ResourceHash resourceId)
{
	if (diffuseMaterials_.find(resourceId) != diffuseMaterials_.cend()) return true;
	if (uiMaterials_.find(resourceId) != uiMaterials_.cend()) return true;

	return false;
}
}    // namespace neko::vk
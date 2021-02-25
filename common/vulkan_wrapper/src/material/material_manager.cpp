#include "vk/material/material_manager.h"

namespace neko::vk
{
MaterialManager::MaterialManager() { MaterialManagerLocator::provide(this); }

void MaterialManager::Clear()
{
	diffuseMaterials_.clear();

	//skyboxMaterialIds_.clear();
	//skyboxMaterials_.clear();

	//trailMaterialIds_.clear();
	//trailMaterials_.clear();

	//particleMaterials_.clear();
	//particleMaterialIds_.clear();

	diffuseMaterials_.emplace(kDefaultMaterialId, DiffuseMaterial());
	diffuseMaterials_[kDefaultMaterialId].CreatePipeline(Vertex::GetVertexInput(0));
}

ResourceHash MaterialManager::AddMaterial(std::string_view materialPath)
{
	const json materialJson         = LoadJson(materialPath);
	const MaterialType materialType = materialJson["type"];
	const ResourceHash resourceId   = HashString(materialJson["name"].get<std::string_view>());
	switch (materialType)
	{
		case MaterialType::DIFFUSE:
		{
			auto& textureManager = TextureManagerLocator::get();
			diffuseMaterials_.emplace(resourceId, DiffuseMaterial());

			// Textures defined in the material's JSON use the relative path to the data folder
			// defined in "BasicEngine::config->dataRootPath"
			if (CheckJsonExists(materialJson, "diffusePath"))
				textureManager.AddTexture2d(materialJson["diffusePath"].get<std::string_view>());

			if (CheckJsonExists(materialJson, "specularPath"))
				textureManager.AddTexture2d(materialJson["specularPath"].get<std::string_view>());

			if (CheckJsonExists(materialJson, "normalPath"))
				textureManager.AddTexture2d(materialJson["normalPath"].get<std::string_view>());

			diffuseMaterials_[resourceId].FromJson(materialJson);
			diffuseMaterials_[resourceId].CreatePipeline(Vertex::GetVertexInput(0));
			break;
		}
	}

	return resourceId;
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
	}

	return resourceId;
}

Material& MaterialManager::GetMaterial(std::string_view materialName)
{
	return GetMaterial(HashString(materialName));
}

Material& MaterialManager::GetMaterial(const ResourceHash resourceId)
{
	if (resourceId == kDefaultMaterialId &&
		diffuseMaterials_.find(resourceId) == diffuseMaterials_.cend())
	{
		diffuseMaterials_.emplace(kDefaultMaterialId, DiffuseMaterial());
		diffuseMaterials_[kDefaultMaterialId].CreatePipeline(Vertex::GetVertexInput(0));
	}

	//Diffuse materials
	if (diffuseMaterials_.find(resourceId) != diffuseMaterials_.cend())
		return diffuseMaterials_[resourceId];

	//Particles materials
	//for (size_t i = 0; i < particleMaterialIDs_.size(); i++)
	//	if (particleMaterialIDs_[i] == resourceID) return particleMaterials_[i];

	//Trails materials
	//for (size_t i = 0; i < trailMaterialIDs_.size(); i++)
	//	if (trailMaterialIDs_[i] == resourceID) return trailMaterials_[i];

	//Skybox materials
	//for (size_t i = 0; i < skyboxMaterialIDs_.size(); i++)
	//	if (skyboxMaterialIDs_[i] == resourceID) return skyboxMaterials_[i];

	return diffuseMaterials_[0];
}

DiffuseMaterial& MaterialManager::GetDiffuseMaterial(std::string_view materialName)
{
	return GetDiffuseMaterial(HashString(materialName));
}

DiffuseMaterial& MaterialManager::GetDiffuseMaterial(ResourceHash resourceId)
{
	if (resourceId == kDefaultMaterialId &&
	    diffuseMaterials_.find(resourceId) == diffuseMaterials_.cend())
	{
		diffuseMaterials_.emplace(kDefaultMaterialId, DiffuseMaterial());
		diffuseMaterials_[kDefaultMaterialId].CreatePipeline(Vertex::GetVertexInput(0));
	}

	return diffuseMaterials_[resourceId];
}

bool MaterialManager::IsMaterialLoaded(std::string_view materialName)
{
	return IsMaterialLoaded(HashString(materialName));
}

bool MaterialManager::IsMaterialLoaded(ResourceHash resourceId)
{
	if (diffuseMaterials_.find(resourceId) != diffuseMaterials_.cend()) return true;

	return false;
}
}    // namespace neko::vk
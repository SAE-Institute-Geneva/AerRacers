#include "vk/material/material_manager.h"

#include "utils/json_utility.h"

namespace neko::vk
{
MaterialManager::MaterialManager()
{
	MaterialManagerLocator::provide(this);
}

void MaterialManager::Clear()
{
	diffuseMaterials_.clear();

	//skyboxMaterialIds_.clear();
	//skyboxMaterials_.clear();

	//trailMaterialIds_.clear();
	//trailMaterials_.clear();

	//particleMaterials_.clear();
	//particleMaterialIds_.clear();

	defaultMaterialIdLoaded_ = false;
	defaultMaterialSkyboxIdLoaded_ = false;
}

ResourceHash MaterialManager::AddMaterial(const std::string& materialPath)
{
	auto materialJson = LoadJson(materialPath);
	const MaterialType materialType = materialJson["type"];

	const ResourceHash resourceId = HashString(materialJson["name"]);
	switch (materialType)
	{
		case MaterialType::DIFFUSE:
		{
			diffuseMaterials_.emplace(resourceId, DiffuseMaterial());

			auto& textureManager = TextureManagerLocator::get();

			//Textures defined in the material's JSON use the relative path to the data folder defined in "BasicEngine::config->dataRootPath"
			if (CheckJsonExists(materialJson, "diffusePath"))
			{
				textureManager.AddTexture2d(materialJson["diffusePath"]);
			}

			if (CheckJsonExists(materialJson, "specularPath"))
			{
				textureManager.AddTexture2d(materialJson["specularPath"]);
			}

			if (CheckJsonExists(materialJson, "normalPath"))
			{
				textureManager.AddTexture2d(materialJson["normalPath"]);
			}

			diffuseMaterials_[resourceId].FromJson(materialJson);
			diffuseMaterials_[resourceId].CreatePipeline(Vertex::GetVertexInput(0));
			break;
		}
	}

	return resourceId;
}

ResourceHash MaterialManager::AddNewMaterial(const std::string& name, MaterialType materialType)
{
	const ResourceHash resourceId = HashString(name);

	switch (materialType)
	{
		case MaterialType::DIFFUSE:
		{
			if (diffuseMaterials_.find(resourceId) != diffuseMaterials_.cend())
				return resourceId;
			diffuseMaterials_.emplace(resourceId, DiffuseMaterial());
			break;
		}
	}
	return resourceId;
}

Material& MaterialManager::GetMaterial(const std::string& materialName)
{
	return GetMaterial(HashString(materialName));
}

Material& MaterialManager::GetMaterial(const ResourceHash resourceId)
{
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

DiffuseMaterial& MaterialManager::GetDiffuseMaterial(const std::string& materialName)
{
	return GetDiffuseMaterial(HashString(materialName));
}

DiffuseMaterial& MaterialManager::GetDiffuseMaterial(ResourceHash resourceId)
{
	return diffuseMaterials_[resourceId];
}

bool MaterialManager::IsMaterialLoaded(const std::string& materialName)
{
	return IsMaterialLoaded(HashString(materialName));
}

bool MaterialManager::IsMaterialLoaded(ResourceHash resourceId)
{
	if (diffuseMaterials_.find(resourceId) != diffuseMaterials_.cend())
		return true;

	return false;
}

ResourceHash MaterialManager::GetDefaultMaterialId()
{
	const ResourceHash defaultMaterialId = HashString(kDefaultMaterialName);
	if (!defaultMaterialIdLoaded_)
	{
		diffuseMaterials_.emplace(defaultMaterialId, DiffuseMaterial());
		diffuseMaterials_[defaultMaterialId].CreatePipeline(Vertex::GetVertexInput(0));

		defaultMaterialIdLoaded_ = true;
	}

	return defaultMaterialId;
}
} 
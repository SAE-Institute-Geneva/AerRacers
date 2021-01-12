#include "vk/material/material_manager.h"

#include "utilities/json_utility.h"

namespace neko::vk
{
MaterialManager::MaterialManager()
{
	diffuseMaterialIds_.reserve(kMaterialsDefaultNum);
	diffuseMaterials_.reserve(kMaterialsDefaultNum);

	//skyboxMaterialIds_.reserve(kMaterialsSkyboxDefaultNum);
	//skyboxMaterials_.reserve(kMaterialsSkyboxDefaultNum);

	//trailMaterialIds_.reserve(kMaterialsDefaultNum);
	//trailMaterials_.reserve(kMaterialsDefaultNum);

	//particleMaterialIds_.reserve(kMaterialsDefaultNum);
	//particleMaterials_.reserve(kMaterialsDefaultNum);

	MaterialManagerLocator::provide(this);
}

void MaterialManager::Resize(size_t newSize)
{
	diffuseMaterialIds_.resize(newSize);
	diffuseMaterials_.resize(newSize);

	//trailMaterialIds_.resize(newSize);
	//trailMaterials_.resize(newSize);

	//particleMaterialIds_.resize(newSize);
	//particleMaterials_.resize(newSize);
}

void MaterialManager::Clear()
{
	diffuseMaterials_.clear();
	diffuseMaterialIds_.clear();

	//skyboxMaterialIds_.clear();
	//skyboxMaterials_.clear();

	//trailMaterialIds_.clear();
	//trailMaterials_.clear();

	//particleMaterials_.clear();
	//particleMaterialIds_.clear();

	defaultMaterialIdLoaded_ = false;
	defaultMaterialSkyboxIdLoaded_ = false;
}

void MaterialManager::AddMaterial(const std::string& materialPath)
{
	auto materialJson = LoadJson(materialPath);
	const MaterialType materialType = materialJson["type"];

	switch (materialType)
	{
		case MaterialType::DIFFUSE:
		{
			diffuseMaterials_.emplace_back(DiffuseMaterial());
			diffuseMaterials_.back().FromJson(materialJson);
			diffuseMaterialIds_.emplace_back(HashString(materialJson["name"]));

			//Textures defined in the material's JSON use the relative path to the data folder defined in "BasicEngine::config->dataRootPath"
			if (CheckJsonExists(materialJson, "diffusePath"))
			{
				//TODO(@Simon) use the texture manager
			}

			if (CheckJsonExists(materialJson, "specularPath"))
			{
				//TODO(@Simon) use the texture manager
			}

			if (CheckJsonExists(materialJson, "normalPath"))
			{
				//TODO(@Simon) use the texture manager
			}

			diffuseMaterials_.back().CreatePipeline(Vertex::GetVertexInput(0));
			break;
		}
	}
}

Material& MaterialManager::GetMaterial(const std::string& materialName)
{
	return GetMaterial(HashString(materialName));
}

Material& MaterialManager::GetMaterial(const ResourceId resourceId)
{
	//Diffuse materials
	for (std::size_t i = 0; i < diffuseMaterialIds_.size(); i++)
		if (diffuseMaterialIds_[i] == resourceId)
			return diffuseMaterials_[i];

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

ResourceId MaterialManager::GetDefaultMaterialId()
{
	if (!defaultMaterialIdLoaded_)
	{
		diffuseMaterials_.emplace_back(DiffuseMaterial());
		diffuseMaterials_.back().CreatePipeline(Vertex::GetVertexInput(0));
		diffuseMaterialIds_.emplace_back(HashString(kDefaultMaterialName));

		defaultMaterialIdLoaded_ = true;
	}

	return HashString(kDefaultMaterialName);
}
}
#pragma once
#include "mathematics/hash.h"
#include "utils/service_locator.h"
#include "vk/material/diffuse_material.h"
#include "vk/pipelines/material_pipeline.h"

namespace neko::vk
{
using ResourceId = XXH64_hash_t;

class IMaterialManager
{
public:
	virtual ~IMaterialManager() = default;

	virtual void Resize(size_t newSize) = 0;
	virtual void Clear() = 0;

	virtual void AddMaterial(const std::string& materialPath) = 0;

	virtual Material& GetMaterial(const std::string& materialName) = 0;
	virtual Material& GetMaterial(ResourceId resourceId) = 0;
	virtual ResourceId GetDefaultMaterialId() = 0;
	//virtual ResourceId GetDefaultSkyboxMaterialId() = 0;
};

class NullMaterialManager : public IMaterialManager
{
public:
	void AddMaterial([[maybe_unused]] const std::string& materialPath) override {}

	void Resize([[maybe_unused]] size_t newSize) override {}
	void Clear() override {}

	Material& GetMaterial([[maybe_unused]] const std::string& materialName) override
	{
		neko_assert(false, "Material Manager is null!")
	}

	Material& GetMaterial([[maybe_unused]] ResourceId resourceId) override
	{
		neko_assert(false, "Material Manager is null!")
	}

	ResourceId GetDefaultMaterialId() override { return 0; }
	//ResourceId GetDefaultSkyboxMaterialId() override { return 0; }
};

class MaterialManager : public IMaterialManager
{
public:
	MaterialManager();

	void Resize(size_t newSize) override;
	void Clear() override;

	void AddMaterial(const std::string& materialPath) override;

	Material& GetMaterial(const std::string& materialName) override;
	Material& GetMaterial(ResourceId resourceId) override;
	ResourceId GetDefaultMaterialId() override;
	//ResourceId GetDefaultSkyboxMaterialId() override;

private:
	inline static const size_t kMaterialsDefaultNum = 100;
	inline static const size_t kMaterialsSkyboxDefaultNum = 2;
	inline static const std::string kDefaultMaterialName = "default_material_name";
	inline static const std::string kDefaultSkyboxMaterialName = "default_skybox_material_name";

	bool defaultMaterialIdLoaded_ = false;
	bool defaultMaterialSkyboxIdLoaded_ = false;

	std::vector<ResourceId> diffuseMaterialIds_;
	std::vector<DiffuseMaterial> diffuseMaterials_;

	//std::vector<ResourceId> skyboxMaterialIds_;
	//std::vector<SkyboxMaterial> skyboxMaterials_;

	//std::vector<ResourceId> trailMaterialIds_;
	//std::vector<TrailMaterial> trailMaterials_;

	//std::vector<ResourceId> particleMaterialIds_;
	//std::vector<ParticleMaterial> particleMaterials_;

	//ImageCube defaultImageCube_;
};

using MaterialManagerLocator = Locator<IMaterialManager, NullMaterialManager>;
}

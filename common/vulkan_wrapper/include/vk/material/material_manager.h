#pragma once
#include "vk/images/texture_manager.h"
#include "vk/material/diffuse_material.h"
#include "vk/pipelines/material_pipeline.h"

namespace neko::vk
{
class IMaterialManager
{
public:
	virtual ~IMaterialManager() = default;

	virtual ResourceHash AddMaterial(const std::string&) = 0;
	virtual ResourceHash AddNewMaterial(const std::string&, MaterialType) = 0;
	virtual void Clear() = 0;

	virtual Material& GetMaterial(const std::string&) = 0;
	virtual Material& GetMaterial(ResourceHash) = 0;
	virtual DiffuseMaterial& GetDiffuseMaterial(const std::string&) = 0;
	virtual DiffuseMaterial& GetDiffuseMaterial(ResourceHash) = 0;
	virtual bool IsMaterialLoaded(const std::string&) = 0;
	virtual bool IsMaterialLoaded(ResourceHash) = 0;
	virtual ResourceHash GetDefaultMaterialId() = 0;
	//virtual ResourceId GetDefaultSkyboxMaterialId() = 0;
};

class NullMaterialManager : public IMaterialManager
{
public:
	ResourceHash AddMaterial(const std::string&) override { return ResourceHash(); }
	ResourceHash AddNewMaterial(const std::string&, MaterialType) override { return ResourceHash(); }
	void Clear() override {}

	Material& GetMaterial(const std::string&) override
	{ neko_assert(false, "Material Manager is null!") }

	Material& GetMaterial(ResourceHash) override
	{ neko_assert(false, "Material Manager is null!") }

	DiffuseMaterial& GetDiffuseMaterial(const std::string&) override
	{ neko_assert(false, "Material Manager is null!") }

	DiffuseMaterial& GetDiffuseMaterial(ResourceHash) override
	{ neko_assert(false, "Material Manager is null!") }

	bool IsMaterialLoaded(const std::string&) override { return false; }
	bool IsMaterialLoaded(ResourceHash) override { return false; }

	ResourceHash GetDefaultMaterialId() override { return 0; }
	//ResourceId GetDefaultSkyboxMaterialId() override { return 0; }
};

class MaterialManager : public IMaterialManager
{
public:
	MaterialManager();

	ResourceHash AddMaterial(const std::string& materialPath) override;
	ResourceHash AddNewMaterial(const std::string& name, MaterialType materialType) override;
	void Clear() override;

	Material& GetMaterial(const std::string& materialName) override;
	Material& GetMaterial(ResourceHash resourceId) override;
	DiffuseMaterial& GetDiffuseMaterial(const std::string& materialName) override;
	DiffuseMaterial& GetDiffuseMaterial(ResourceHash resourceId) override;
	bool IsMaterialLoaded(const std::string& materialName) override;
	bool IsMaterialLoaded(ResourceHash resourceId) override;
	ResourceHash GetDefaultMaterialId() override;
	//ResourceId GetDefaultSkyboxMaterialId() override;

private:
	inline static const size_t kMaterialsDefaultNum = 100;
	inline static const size_t kMaterialsSkyboxDefaultNum = 2;
	inline static const std::string kDefaultMaterialName = "default_material_name";
	inline static const std::string kDefaultSkyboxMaterialName = "default_skybox_material_name";

	bool defaultMaterialIdLoaded_ = false;
	bool defaultMaterialSkyboxIdLoaded_ = false;

	std::map<ResourceHash, DiffuseMaterial> diffuseMaterials_;

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

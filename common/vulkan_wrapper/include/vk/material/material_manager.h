#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/images/texture_manager.h"
#include "vk/material/diffuse_material.h"

namespace neko::vk
{
//constexpr size_t kMaterialsDefaultNum                 = 100;
//constexpr size_t kMaterialsSkyboxDefaultNum           = 2;
constexpr std::string_view kDefaultMaterialName       = "DefaultMaterialName";
//constexpr std::string_view kDefaultSkyboxMaterialName = "DefaultSkyboxMaterialName";
constexpr StringHash kDefaultMaterialId               = HashString(kDefaultMaterialName);
//constexpr StringHash kDefaultSkyboxMaterialId         = HashString(kDefaultSkyboxMaterialName);

class IMaterialManager
{
public:
	virtual ~IMaterialManager() = default;

	[[maybe_unused]] virtual ResourceHash AddMaterial(std::string_view) = 0;
	virtual ResourceHash AddNewMaterial(std::string_view, MaterialType) = 0;
	virtual void Clear()                                                = 0;

	virtual Material& GetMaterial(std::string_view)               = 0;
	virtual Material& GetMaterial(ResourceHash)                   = 0;
	virtual DiffuseMaterial& GetDiffuseMaterial(std::string_view) = 0;
	virtual DiffuseMaterial& GetDiffuseMaterial(ResourceHash)     = 0;
	virtual bool IsMaterialLoaded(std::string_view)               = 0;
	virtual bool IsMaterialLoaded(ResourceHash)                   = 0;
};

class NullMaterialManager : public IMaterialManager
{
public:
	ResourceHash AddMaterial(std::string_view) override { return {}; }
	ResourceHash AddNewMaterial(std::string_view, MaterialType) override { return {}; }

	void Clear() override {}

	Material& GetMaterial(std::string_view) override
	{
		neko_assert(false, "Material Manager is null!");
	}

	Material& GetMaterial(ResourceHash) override {neko_assert(false, "Material Manager is null!");}

	DiffuseMaterial& GetDiffuseMaterial(std::string_view) override
	{
		neko_assert(false, "Material Manager is null!");
	}

	DiffuseMaterial& GetDiffuseMaterial(ResourceHash) override
	{
		neko_assert(false, "Material Manager is null!");
	}

	bool IsMaterialLoaded(std::string_view) override { return false; }
	bool IsMaterialLoaded(ResourceHash) override { return false; }
};

class MaterialManager final : public IMaterialManager
{
public:
	MaterialManager();

	ResourceHash AddMaterial(std::string_view materialPath) override;
	ResourceHash AddNewMaterial(std::string_view name, MaterialType materialType) override;
	void Clear() override;

	Material& GetMaterial(std::string_view materialName) override;
	Material& GetMaterial(ResourceHash resourceId) override;
	DiffuseMaterial& GetDiffuseMaterial(std::string_view materialName) override;
	DiffuseMaterial& GetDiffuseMaterial(ResourceHash resourceId) override;
	bool IsMaterialLoaded(std::string_view materialName) override;
	bool IsMaterialLoaded(ResourceHash resourceId) override;

private:
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
}    // namespace neko::vk

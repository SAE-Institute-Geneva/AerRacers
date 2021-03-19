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
#include "graphics/tinyobj_loader.h"

#include "vk/material/diffuse_material.h"
#include "vk/models/model.h"

namespace neko::vk
{
using ModelId                      = sole::uuid;
constexpr ModelId INVALID_MODEL_ID = sole::uuid();

class ModelLoader
{
public:
	enum ModelFlags : std::uint8_t
	{
		NONE          = 0u,
		LOADED        = 1u << 0u,
		ERROR_LOADING = 1u << 1u,
	};

	ModelLoader(std::string_view path, ModelId modelId);

	ModelLoader(ModelLoader&& other) noexcept;
	ModelLoader(const ModelLoader& other) = delete;
	ModelLoader& operator=(const ModelLoader& other) = delete;

	void Start();
	void Update();

	[[nodiscard]] ModelId GetModelId() const { return modelId_; }
	[[nodiscard]] const Model* GetModel() const { return &model_; }

	[[nodiscard]] bool IsDone() const { return flags_ & LOADED; }
	[[nodiscard]] bool HasErrors() const { return flags_ & ERROR_LOADING; }

private:
	/// Uses the assimp importer to load model from disk
	void LoadModel();

	/// Process all the node of the aiScene, aka the meshes
	void ProcessModel();
	void LoadMaterialTextures(const tinyobj::material_t& mat,
		Mesh& mesh,
		DiffuseMaterial::TextureMaps textureType,
		std::string_view texName);

	/// Method called on the Render thread to create the VAOs of the meshes
	void UploadMeshesToVk();

	friend class ModelManager;
	std::string path_;
	std::string directory_;

	ModelId modelId_ = INVALID_MODEL_ID;
	tinyobj::attrib_t attrib_;
	std::vector<tinyobj::shape_t> shapes_;
	std::vector<tinyobj::material_t> materials_;
	Model model_;

	Job loadModelJob_;
	Job processModelJob_;
	Job uploadJob_;

	std::uint8_t flags_       = NONE;
	std::uint8_t textureMaps_ = 0;

	ResourceHash diffuseId_  = 0;
	ResourceHash specularId_ = 0;
	ResourceHash normalId_   = 0;
};
}    // namespace neko::vk

MAKE_HASHABLE(neko::Vec2f, t.x, t.y)
MAKE_HASHABLE(neko::Vec3f, t.x, t.y, t.z)
MAKE_HASHABLE(neko::vk::Vertex, t.position, t.normal, t.texCoords, t.tangent, t.bitangent)

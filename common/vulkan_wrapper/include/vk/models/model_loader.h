#pragma once
#include "assimp/Importer.hpp"
#include "assimp/mesh.h"
#include "assimp/scene.h"

#include "vk/models/model.h"

namespace neko::vk
{
using ModelId = sole::uuid;
const ModelId INVALID_MODEL_ID = sole::uuid();

class ModelLoader
{
public:
	enum ModelFlags : std::uint8_t
	{
		NONE = 0u,
		LOADED = 1u << 0u,
		ERROR_LOADING = 1u << 1u,

	};

	ModelLoader(Assimp::Importer& importer, std::string path, ModelId modelId);

	ModelLoader(ModelLoader&& other) noexcept ;
	ModelLoader(const ModelLoader& other) = delete;
	ModelLoader& operator=(const ModelLoader& other) = delete;

	void Start();
	void Update();

	[[nodiscard]] bool IsDone() const { return flags_ & LOADED; }
	[[nodiscard]] ModelId GetModelId() const { return modelId_; }
	[[nodiscard]] const Model* GetModel() const { return &model_; }
	[[nodiscard]] bool HasErrors() const { return flags_ & ERROR_LOADING; }

private:
	/**
	 * \brief Uses the assimp importer to load model from disk
	 */
	void LoadModel();
	/**
	 * \brief Process all the node of the aiScene, aka the meshes
	 */
	void ProcessModel();
	void ProcessNode(aiNode* node);
	void ProcessMesh(Mesh& mesh, const aiMesh* aMesh);
	static void LoadMaterialTextures(
			const aiMaterial* material,
			aiTextureType textureType,
			std::string_view directory,
			Mesh& mesh);
	/**
	 * \brief method called on the Render thread to create the VAOs of the meshes
	 */
	void UploadMeshesToVk();

	std::string path_;
	std::string directoryPath_;

	ModelId modelId_ = INVALID_MODEL_ID;

	std::reference_wrapper<Assimp::Importer> importer_;
	const aiScene* scene = nullptr;
	Model model_;

	Job loadModelJob_;
	Job processModelJob_;
	Job uploadMeshesToVkJob_;

	std::uint8_t flags_ = NONE;
};
}

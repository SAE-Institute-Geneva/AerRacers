/*
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
 */
#include "gl/model.h"

#include <assimp/postprocess.h>

#include "engine/engine.h"
#include "graphics/graphics.h"

#include "io_system.h"

#ifdef EASY_PROFILE_USE
	#include "easy/profiler.h"
#endif

namespace neko::gl
{
/*
void OldModel::Draw(const gl::Shader& shader)
{
    for (auto& mesh : meshes_)
        mesh.Draw(shader);
}

void OldModel::Destroy()
{
    for (auto& mesh : meshes_)
        mesh.Destroy();
    meshes_.clear();
    processModelJob_.Reset();
}


OldModel::OldModel() : processModelJob_([this] {
    ProcessModel();
}),
filesystem_(BasicEngine::GetInstance()->GetFilesystem())
{
}

void OldModel::LoadModel(std::string_view path)
{
    path_ = path;
    directory_ = path.substr(0, path.find_last_of('/'));
    logDebug(fmt::format("ASSIMP: Loading model: {}", path_));
    BasicEngine::GetInstance()->ScheduleJob(&processModelJob_, JobThreadType::OTHER_THREAD);
}

bool OldModel::IsLoaded() const
{

    if (!processModelJob_.IsDone())
    {
        return false;
    }
    for (const auto& mesh : meshes_)
    {
        if (!mesh.IsLoaded())
            return false;
    }

    return true;
}

void OldModel::ProcessModel()
{

#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Process 3d Model");
    EASY_BLOCK("Load 3d Model");
#endif
    Assimp::Importer import;
    const aiScene* scene = nullptr;

    Job loadingModelJob = Job([this, &import, &scene] {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Model Disk Load");
#endif
        //assimp delete automatically the IO System
        auto* ioSystem = new NekoIOSystem(filesystem_.get());
        import.SetIOHandler(ioSystem);

        scene = import.ReadFile(path_.data(),
                                aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
                                aiProcess_CalcTangentSpace);
    });
    BasicEngine::GetInstance()->ScheduleJob(&loadingModelJob, JobThreadType::RESOURCE_THREAD);

    loadingModelJob.Join();

#ifdef EASY_PROFILE_USE
    EASY_END_BLOCK;
#endif
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        logDebug(fmt::format("[ERROR] ASSIMP {}", import.GetErrorString()));
        return;
    }
    meshes_.reserve(scene->mNumMeshes);
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Process Nodes");
#endif
    ProcessNode(scene->mRootNode, scene);
#ifdef EASY_PROFILE_USE
    EASY_END_BLOCK;
    EASY_BLOCK("Schedule Mesh Job");
#endif
    for (auto& mesh : meshes_)
    {
        mesh.Init();
    }

}

void OldModel::ProcessNode(aiNode* node, const aiScene* scene)
{
    // process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        meshes_.emplace_back();
        auto& mesh = meshes_.back();

        aiMesh* assMesh = scene->mMeshes[node->mMeshes[i]];
        mesh.ProcessMesh(assMesh, scene, directory_);
    }
    // then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}
*/

void Model::BindTextures(size_t meshIndex, const Shader& shader) const
{
	glCheckError();
	std::uint8_t usedMaps    = 0;
	const assimp::Mesh& mesh = meshes_[meshIndex];
	for (const auto& texture : mesh.textures)
	{
		// activate proper texture unit before binding
		// retrieve texture number (the N in diffuse_textureN)
		std::uint8_t number = 0;
		std::string name;
		switch (texture.type)
		{
			case aiTextureType_DIFFUSE:
				name   = "diffuse";
				number = 0;
				break;
			case aiTextureType_SPECULAR:
				name   = "specular";
				number = 1;
				break;
			case aiTextureType_HEIGHT:
			case aiTextureType_NORMALS:
				name   = "normal";
				number = 2;
				break;
			case aiTextureType_EMISSIVE:
				name   = "emissive";
				number = 3;
				break;
			default:;
		}

		usedMaps |= 1u << number;
		glActiveTexture(GL_TEXTURE0 + number);
		shader.SetInt(std::string("material.").append(name), number);
		glBindTexture(GL_TEXTURE_2D, texture.textureName);
	}

	shader.SetFloat("material.shininess", mesh.specularExponent);
	shader.SetUInt("usedMaps", usedMaps);
	glActiveTexture(GL_TEXTURE0);
	glCheckError();
}

void Model::Draw(const Shader& shader) const
{
	for (size_t meshIndex = 0; meshIndex < meshes_.size(); meshIndex++)
	{
		BindTextures(meshIndex, shader);
		const auto& mesh = meshes_[meshIndex];

		// draw mesh
		glBindVertexArray(mesh.VAO);
		glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, nullptr);
		glBindVertexArray(0);
	}
}

void Model::Destroy()
{
	for (auto& mesh : meshes_) { glDeleteVertexArrays(1, &mesh.VAO); }
}

ModelManager::ModelManager()
{
	importer_.SetIOHandler(new NekoIOSystem(BasicEngine::GetInstance()->GetFilesystem()));
	ModelManagerLocator::provide(this);
}

void ModelManager::Init() {}

void ModelManager::Update(seconds)
{
	while (!modelLoaders_.empty())
	{
		auto& modelLoader = modelLoaders_.front();
		modelLoader.Update();
		if (modelLoader.HasErrors()) { modelLoaders_.pop(); }
		else if (modelLoader.IsDone())
		{
			modelMap_[modelLoader.GetModelId()] = *modelLoader.GetModel();
			modelLoaders_.pop();
		}
		else
		{
			break;
		}
	}
}

void ModelManager::Destroy()
{
	for (auto& model : modelMap_) model.second.Destroy();
}

const Model* ModelManager::GetModel(ModelId modelId) const
{
	if (modelId == INVALID_MODEL_ID) return nullptr;

	const auto it = modelMap_.find(modelId);
	if (it != modelMap_.end()) return &it->second;
	return nullptr;
}

ModelId ModelManager::LoadModel(std::string_view path)
{
	const auto it = modelPathMap_.find(path.data());
	if (it != modelPathMap_.end()) { return it->second; }

	const std::string metaPath = fmt::format("{}.meta", path);
	auto metaJson              = LoadJson(metaPath);
	ModelId modelId            = INVALID_MODEL_ID;
	if (CheckJsonExists(metaJson, "uuid"))
	{
		modelId = sole::rebuild(metaJson["uuid"].get<std::string>());
	}
	else
	{
		logDebug(fmt::format("[Error] Could not find model id in json file: {}", metaPath));
		return modelId;
	}

	modelLoaders_.push(ModelLoader(path, modelId));
	modelLoaders_.back().Start();
	modelPathMap_.emplace(path, modelId);
	return modelId;
}

std::string ModelManager::GetModelName(ModelId modelId)
{
	for (auto& it : modelPathMap_)
	{
		if (it.second == modelId)
		{
			std::size_t startName         = it.first.find_last_of('/') + 1;
			std::size_t endName           = it.first.find_first_of('.');
			std::size_t size              = endName - startName;
			std::string name              = it.first.substr(startName, size);
			return name;
		}
	}

	return "";
}

std::string_view ModelManager::GetModelPath(ModelId modelId)
{
	for (auto& it : modelPathMap_)
		if (it.second == modelId)
			return it.first;

	return "";
}

ModelLoader::ModelLoader(std::string_view path, ModelId modelId)
   : path_(path),
	 modelId_(modelId),
	 loadModelJob_([this]() { LoadModel(); }),
	 processModelJob_([this]() { ProcessModel(); }),
	 uploadJob_([this]() { UploadMeshesToGl(); })
{}

ModelLoader::ModelLoader(ModelLoader&& modelLoader) noexcept
   : path_(std::move(modelLoader.path_)),
	 modelId_(modelLoader.modelId_),
	 loadModelJob_([this]() { LoadModel(); }),
	 processModelJob_([this]() { ProcessModel(); }),
	 uploadJob_([this]() { UploadMeshesToGl(); })
{}

void ModelLoader::Start()
{
    importer_.SetIOHandler(new NekoIOSystem(BasicEngine::GetInstance()->GetFilesystem()));
	directoryPath_ = path_.substr(0, path_.find_last_of('/'));
    BasicEngine::GetInstance()->ScheduleJob(&loadModelJob_, JobThreadType::RESOURCE_THREAD);
}

void ModelLoader::Update()
{
	if (flags_ & ERROR_LOADING) return;

	if (!(flags_ & LOADED) && processModelJob_.IsDone())
    {
        const auto& textureManager = TextureManagerLocator::get();
        bool isLoaded = true;

        // load textures if possible
		for (auto& mesh : model_.meshes_)
		{
			size_t loadedTextureCount = 0;
			for (auto& texture : mesh.textures)
			{
				if (texture.textureId != INVALID_TEXTURE_ID &&
					texture.textureName == INVALID_TEXTURE_NAME)
				{
					const auto* texturePtr = textureManager.GetTexture(texture.textureId);
					if (texturePtr != nullptr) { texture.textureName = texturePtr->name; }
				}

				if (texture.textureId != INVALID_TEXTURE_ID &&
					texture.textureName != INVALID_TEXTURE_NAME)
				{
					loadedTextureCount++;
				}
			}

			if (loadedTextureCount < mesh.textures.size()) { isLoaded = false; }
		}

		if (isLoaded && uploadJob_.IsDone()) { flags_ = flags_ | LOADED; }
	}
}

void ModelLoader::LoadModel()
{
	scene_ = importer_.ReadFile(path_.data(),
		aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
			aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);

	if (!scene_ || scene_->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene_->mRootNode)
	{
		flags_ = ERROR_LOADING;
		logDebug(fmt::format("[ERROR] ASSIMP {}", importer_.GetErrorString()));
		return;
	}

	BasicEngine::GetInstance()->ScheduleJob(&processModelJob_, JobThreadType::OTHER_THREAD);
}

void ModelLoader::ProcessModel()
{
	model_.meshes_.reserve(scene_->mNumMeshes);
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Process Nodes");
#endif
	ProcessNode(scene_->mRootNode);
	RendererLocator::get().AddPreRenderJob(&uploadJob_);
}

void ModelLoader::ProcessNode(aiNode* node)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		model_.meshes_.push_back({});
		auto& mesh = model_.meshes_.back();
		ProcessMesh(mesh, scene_->mMeshes[node->mMeshes[i]]);
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++) { ProcessNode(node->mChildren[i]); }
}

void ModelLoader::ProcessMesh(assimp::Mesh& mesh, const aiMesh* aMesh)
{
	mesh.aabb.lowerLeftBound  = Vec3f(aMesh->mAABB.mMin);
	mesh.aabb.upperRightBound = Vec3f(aMesh->mAABB.mMax);
	for (unsigned int i = 0; i < aMesh->mNumVertices; i++)
	{
		assimp::Vertex vertex;

		// process vertex positions, normals and texture coordinates
		vertex.position = Vec3f(aMesh->mVertices[i]);
		vertex.normal   = Vec3f(aMesh->mNormals[i]);

		if (aMesh->mTangents != nullptr)
		{
			vertex.tangent   = Vec3f(aMesh->mTangents[i]);
			vertex.bitangent = Vec3f(aMesh->mBitangents[i]);
		}

		if (aMesh->mTextureCoords[0]) vertex.texCoords = Vec2f(aMesh->mTextureCoords[0][i]);
		else
			vertex.texCoords = Vec2f(0.0f, 0.0f);

		mesh.vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < aMesh->mNumFaces; i++)
	{
		const aiFace face = aMesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			mesh.indices.push_back(face.mIndices[j]);
	}

	// process material
	if (aMesh->mMaterialIndex >= 0)
	{
		const aiMaterial* material = scene_->mMaterials[aMesh->mMaterialIndex];
		material->Get(AI_MATKEY_SHININESS, mesh.specularExponent);
		for (int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			LoadMaterialTextures(material, static_cast<aiTextureType>(i), directoryPath_, mesh);
		}
	}
}

void ModelLoader::LoadMaterialTextures(const aiMaterial* material,
	aiTextureType textureType,
	std::string_view directory,
	assimp::Mesh& mesh)
{
	auto& textureManager = TextureManagerLocator::get();
	const auto count     = material->GetTextureCount(textureType);
	for (unsigned int i = 0; i < count; i++)
	{
		aiString textureName;
		material->GetTexture(textureType, i, &textureName);

		const auto textureId =
			textureManager.LoadTexture(fmt::format("{}/{}", directory, textureName.C_Str()));

		mesh.textures.push_back({textureId, INVALID_TEXTURE_NAME, textureType});
	}
}

void ModelLoader::UploadMeshesToGl()
{
	for (auto& mesh : model_.meshes_)
	{
#ifdef EASY_PROFILE_USE
		EASY_BLOCK("Create Mesh VAO");
		EASY_BLOCK("Generate Buffers");
		EASY_BLOCK("Generate VAO");
#endif
		glCheckError();
		glGenVertexArrays(1, &mesh.VAO);
#ifdef EASY_PROFILE_USE
		EASY_END_BLOCK;
		EASY_BLOCK("Generate VBO");
#endif
		glGenBuffers(1, &mesh.VBO);
		glCheckError();
#ifdef EASY_PROFILE_USE
		EASY_END_BLOCK;
		EASY_BLOCK("Generate EBO");
#endif
		glGenBuffers(1, &mesh.EBO);
		glCheckError();
#ifdef EASY_PROFILE_USE
		EASY_END_BLOCK;
		EASY_END_BLOCK;
		EASY_BLOCK("Copy Buffers");
#endif
		glBindVertexArray(mesh.VAO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
		glCheckError();
		glBufferData(GL_ARRAY_BUFFER,
			mesh.vertices.size() * sizeof(assimp::Vertex),
			&mesh.vertices[0],
			GL_STATIC_DRAW);
		glCheckError();
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh.indices.size() * sizeof(unsigned int),
			&mesh.indices[0],
			GL_STATIC_DRAW);
		glCheckError();
#ifdef EASY_PROFILE_USE
		EASY_END_BLOCK;
		EASY_BLOCK("Vertex Attrib");
#endif
		// vertex positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(assimp::Vertex),
			(void*) offsetof(assimp::Vertex, position));
		glCheckError();

		// vertex texture coords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(assimp::Vertex),
			(void*) offsetof(assimp::Vertex, texCoords));
		glCheckError();

		// vertex normals
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(assimp::Vertex),
			(void*) offsetof(assimp::Vertex, normal));
		glCheckError();

		// vertex tangent
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(assimp::Vertex),
			(void*) offsetof(assimp::Vertex, tangent));
		glCheckError();

		// vertex bitangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(assimp::Vertex),
			(void*) offsetof(assimp::Vertex, bitangent));
		glCheckError();

		glBindVertexArray(0);
		glCheckError();
	}
}
}

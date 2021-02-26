#include "vk/models/model_loader.h"

#include "assimp/postprocess.h"

#include "mathematics/quaternion.h"
#include "utils/file_utility.h"

#include "vk/material/material_manager.h"
#include "vk/models/io_system.h"
#include "vk/vk_resources.h"

namespace neko::vk
{
ModelLoader::ModelLoader(std::string_view path, ModelId modelId)
   : path_(path),
	 modelId_(modelId),
	 loadModelJob_([this]() { LoadModel(); }),
	 processModelJob_([this]() { ProcessModel(); }),
	 uploadJob_([this]() { UploadMeshesToVk(); })
{
	importer_.SetIOHandler(new VkIoSystem(BasicEngine::GetInstance()->GetFilesystem()));
}

ModelLoader::ModelLoader(ModelLoader&& other) noexcept
   : path_(std::move(other.path_)),
	 modelId_(other.modelId_),
	 loadModelJob_([this]() { LoadModel(); }),
	 processModelJob_([this]() { ProcessModel(); }),
	 uploadJob_([this]() { UploadMeshesToVk(); })
{}

void ModelLoader::Start()
{
	directoryPath_ = path_.substr(0, path_.find_last_of('/'));
	BasicEngine::GetInstance()->ScheduleJob(&loadModelJob_, JobThreadType::RESOURCE_THREAD);
}

void ModelLoader::Update()
{
	if (flags_ & ERROR_LOADING) return;
	if (!(flags_ & LOADED) && processModelJob_.IsDone())
	{
		const auto& textureManager = TextureManagerLocator::get();
		auto& materialManager = MaterialManagerLocator::get();
		bool isLoaded = false;

		// load textures if possible
		for (auto& mesh : model_.meshes_)
		{
			std::uint8_t loadedTextures = 0;
			DiffuseMaterial& material = materialManager.GetDiffuseMaterial(mesh.materialId_);
			if (diffuseId_ != INVALID_TEXTURE_ID)
			{
				const auto* texturePtr = textureManager.GetTexture(diffuseId_);
				if (texturePtr)
				{
					material.SetDiffuse(*texturePtr);
					loadedTextures |= DIFFUSE;
				}
			}

			if (specularId_ != INVALID_TEXTURE_ID)
			{
				const auto* texturePtr = textureManager.GetTexture(specularId_);
				if (texturePtr)
				{
					material.SetSpecular(*texturePtr);
					loadedTextures |= SPECULAR;
				}
			}

			if (normalId_ != INVALID_TEXTURE_ID)
			{
				const auto* texturePtr = textureManager.GetTexture(normalId_);
				if (texturePtr)
				{
					material.SetNormal(*texturePtr);
					loadedTextures |= NORMAL;
				}
			}

			if (loadedTextures == textureMaps_) isLoaded = true;
		}

		if (isLoaded && uploadJob_.IsDone()) flags_ |= LOADED;
	}
}

void ModelLoader::LoadModel()
{
	const Configuration& config = BasicEngine::GetInstance()->GetConfig();
	std::uint32_t sceneFlags    = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
	                           aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes;
	scene_ = importer_.ReadFile(config.dataRootPath + path_, sceneFlags);
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
	for (std::uint32_t i = 0; i < node->mNumMeshes; i++)
	{
		auto& mesh = model_.meshes_.emplace_back();
		ProcessMesh(mesh, scene_->mMeshes[node->mMeshes[i]]);
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++) ProcessNode(node->mChildren[i]);
}

void ModelLoader::ProcessMesh(Mesh& mesh, const aiMesh* aMesh)
{
	mesh.minExtents_ = Vec3f(aMesh->mAABB.mMin);
	mesh.maxExtents_ = Vec3f(aMesh->mAABB.mMax);

	std::vector<Vertex> vertices(aMesh->mNumVertices);
	for (unsigned int i = 0; i < aMesh->mNumVertices; i++)
	{
		Vertex vertex;

		// process vertex positions, normals and texture coordinates
		vertex.position = Vec3f(aMesh->mVertices[i]);
		if (GetFilenameExtension(path_) == ".fbx")
			vertex.position = Quaternion::AngleAxis(degree_t(90.0f), Vec3f::left) * vertex.position;
		vertex.normal = Vec3f(aMesh->mNormals[i]);

		if (aMesh->mTangents)
		{
			vertex.tangent   = Vec3f(aMesh->mTangents[i]);
			vertex.bitangent = Vec3f(aMesh->mBitangents[i]);
		}

		if (aMesh->mTextureCoords[0])
			vertex.texCoords = Vec2f(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);
		else
			vertex.texCoords = Vec2f(0.0f, 0.0f);

		vertices[i] = vertex;
	}
	mesh.SetVertices(vertices);

	std::vector<std::uint32_t> indices;
	for (unsigned int i = 0; i < aMesh->mNumFaces; i++)
	{
		// process indices
		const aiFace face = aMesh->mFaces[i];
		for (std::uint32_t j = 0; j < face.mNumIndices; j++) indices.push_back(face.mIndices[j]);
	}
	mesh.SetIndices(indices);

	if (aMesh->mMaterialIndex >= 0)
	{
		// process material
		const aiMaterial* material = scene_->mMaterials[aMesh->mMaterialIndex];
		float specularExp;
		aiColor4D diffuse;
		material->Get(AI_MATKEY_SHININESS, specularExp);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		Color4 col = Color4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);

		auto& materialManager = MaterialManagerLocator::get();
		const std::string matName = GetFilename(path_) + std::to_string(aMesh->mMaterialIndex);
		if (materialManager.IsMaterialLoaded(matName))
		{
			mesh.materialId_ = HashString(matName);
			return;
		}

		mesh.materialId_ = materialManager.AddNewMaterial(matName, MaterialType::DIFFUSE);
		materialManager.GetDiffuseMaterial(mesh.materialId_).SetSpecularExponent(specularExp);
		materialManager.GetDiffuseMaterial(mesh.materialId_).SetColor(col);
		for (int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
			LoadMaterialTextures(material, static_cast<aiTextureType>(i), directoryPath_, mesh);
	}
}

void ModelLoader::UploadMeshesToVk() {}

void ModelLoader::LoadMaterialTextures(
	const aiMaterial* material, aiTextureType textureType, std::string_view directory, Mesh& mesh)
{
	auto& textureManager  = TextureManagerLocator::get();
	auto& materialManager = MaterialManagerLocator::get();
	const auto count      = material->GetTextureCount(textureType);
	for (std::uint32_t i = 0; i < count; i++)
	{
		aiString textureName;
		material->GetTexture(textureType, i, &textureName);

		std::size_t startPos       = 0;
		std::string textureNameStr = textureName.C_Str();
		while ((startPos = textureNameStr.find('\\', startPos)) != std::string::npos)
		{
			textureNameStr.replace(startPos, 1, "/");
			startPos += 1;
		}

		const Configuration& config  = BasicEngine::GetInstance()->GetConfig();
		const ResourceHash textureId = textureManager.AddTexture(
			fmt::format("{}/{}.ktx", config.dataRootPath + directory.data(), textureNameStr));
		switch (textureType)
		{
			case aiTextureType_DIFFUSE:
				textureMaps_ |= DIFFUSE;
				diffuseId_ = textureId;
				//materialManager.GetDiffuseMaterial(mesh.materialId_)
				//	.SetDiffuse(textureManager.GetImage2d(textureId));
				break;
			case aiTextureType_SPECULAR:
				textureMaps_ |= SPECULAR;
				specularId_ = textureId;
				//materialManager.GetDiffuseMaterial(mesh.materialId_)
				//	.SetSpecular(textureManager.GetImage2d(textureId));
				break;
			case aiTextureType_EMISSIVE: break;
			case aiTextureType_HEIGHT:
			case aiTextureType_NORMALS:
				textureMaps_ |= NORMAL;
				normalId_ = textureId;
				//materialManager.GetDiffuseMaterial(mesh.materialId_)
				//	.SetNormal(textureManager.GetImage2d(textureId));
				break;
			case aiTextureType_NONE:
			case aiTextureType_UNKNOWN:
			case aiTextureType_AMBIENT:
			case aiTextureType_SHININESS:
			case aiTextureType_OPACITY:
			case aiTextureType_DISPLACEMENT:
			case aiTextureType_LIGHTMAP:
			case aiTextureType_REFLECTION:
			case aiTextureType_BASE_COLOR:
			case aiTextureType_NORMAL_CAMERA:
			case aiTextureType_EMISSION_COLOR:
			case aiTextureType_METALNESS:
			case aiTextureType_DIFFUSE_ROUGHNESS:
			case aiTextureType_AMBIENT_OCCLUSION:
			case _aiTextureType_Force32Bit: logDebug("Unsupported texture type"); break;
		}
	}
}
}    // namespace neko::vk
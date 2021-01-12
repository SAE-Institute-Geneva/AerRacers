#include "vk/models/model_loader.h"

#include <utility>

#include "assimp/postprocess.h"
#include "utils/file_utility.h"
#include "vk/graphics.h"

namespace neko::vk
{
ModelLoader::ModelLoader(Assimp::Importer& importer, std::string path, ModelId modelId)
	: path_(std::move(path)),
	modelId_(modelId),
	importer_(importer),
	loadModelJob_([this]() { LoadModel(); }),
	processModelJob_([this]() { ProcessModel(); }),
	uploadMeshesToVkJob_([this]() { UploadMeshesToVk(); })
{}

ModelLoader::ModelLoader(ModelLoader&& other) noexcept
	: path_(std::move(other.path_)),
	  modelId_(other.modelId_),
	  importer_(other.importer_),
	  loadModelJob_([this]() { LoadModel(); }),
	  processModelJob_([this]() { ProcessModel(); }),
	  uploadMeshesToVkJob_([this]() { UploadMeshesToVk(); })
{}

void ModelLoader::Start()
{
	directoryPath_ = path_.substr(0, path_.find_last_of('/'));
	//LoadModel();
	BasicEngine::GetInstance()->ScheduleJob(&loadModelJob_, JobThreadType::RESOURCE_THREAD);
}

void ModelLoader::Update()
{
	if (flags_ & ERROR_LOADING)
	{
		return;
	}

	if (!(flags_ & LOADED) && processModelJob_.IsDone())
	{
		if (uploadMeshesToVkJob_.IsDone())
		{
			flags_ = flags_ | LOADED;
		}
	}
}

void ModelLoader::LoadModel()
{
	const auto& config = BasicEngine::GetInstance()->GetConfig();
	scene = importer_.get().ReadFile(config.dataRootPath + path_,
	                                 aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
	                                 aiProcess_CalcTangentSpace);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		flags_ = ERROR_LOADING;
		logDebug(fmt::format("[ERROR] ASSIMP {}", importer_.get().GetErrorString()));
		return;
	}

	//ProcessModel();
	BasicEngine::GetInstance()->ScheduleJob(&processModelJob_, JobThreadType::OTHER_THREAD);
}

void ModelLoader::ProcessModel()
{
	model_.meshes_.reserve(scene->mNumMeshes);
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Process Nodes");
#endif
	ProcessNode(scene->mRootNode);

	flags_ = flags_ | LOADED; //TODO for testing only
	RendererLocator::get().AddPreRenderJob(&uploadMeshesToVkJob_);
}

void ModelLoader::ProcessNode(aiNode* node)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		model_.meshes_.emplace_back();
		auto& mesh = model_.meshes_.back();
		ProcessMesh(mesh, scene->mMeshes[node->mMeshes[i]]);
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i]);
	}
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
		{
			vertex.position = Quaternion::AngleAxis(degree_t(90.0f), Vec3f::left) * vertex.position;
		}

		vertex.normal = Vec3f(aMesh->mNormals[i]);

		//TODO: why is tangent sometimes null even with CalcTangent
		if (aMesh->mTangents != nullptr)
		{
			vertex.tangent = Vec3f(aMesh->mTangents[i]);
			vertex.bitangent = Vec3f(aMesh->mBitangents[i]);
		}

		if (aMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
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
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	mesh.SetIndices(indices);

	if (aMesh->mMaterialIndex >= 0)
	{
		// process material
		const aiMaterial* material = scene->mMaterials[aMesh->mMaterialIndex];
		float specularExp;
		aiColor4D diffuse;
		material->Get(AI_MATKEY_SHININESS, specularExp);
		material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
		Color4 col = Color4(diffuse.r, diffuse.g, diffuse.b, diffuse.a);

		auto& materialManager = MaterialManagerLocator::get();
		const auto& matName = GetFilename(path_) + std::to_string(aMesh->mMaterialIndex);
		if (materialManager.IsMaterialLoaded(matName))
		{
			mesh.materialId_ = HashString(matName);
			return;
		}

		mesh.materialId_ = materialManager.AddNewMaterial(matName, MaterialType::DIFFUSE);
		materialManager.GetDiffuseMaterial(mesh.materialId_).SetSpecularExponent(specularExp);
		materialManager.GetDiffuseMaterial(mesh.materialId_).SetColor(col);
		for (int i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
		{
			LoadMaterialTextures(material, static_cast<aiTextureType>(i), directoryPath_, mesh);
		}
	}
}

void ModelLoader::UploadMeshesToVk()
{

}

void ModelLoader::LoadMaterialTextures(
		const aiMaterial* material,
		aiTextureType textureType,
		std::string_view directory,
		Mesh& mesh)
{
	auto& textureManager = TextureManagerLocator::get();
	auto& materialManager = MaterialManagerLocator::get();
	const auto count = material->GetTextureCount(textureType);
	for (unsigned int i = 0; i < count; i++)
	{
		aiString textureName;
		material->GetTexture(textureType, i, &textureName);

		size_t start_pos = 0;
		std::string textureNameStr = std::string(textureName.C_Str());
		while((start_pos = textureNameStr.find('\\', start_pos)) != std::string::npos)
		{
			textureNameStr.replace(start_pos, 1, "/");
			start_pos += 1;
		}

		const auto& config = BasicEngine::GetInstance()->GetConfig();
		const auto textureId = textureManager.AddTexture2d(
				fmt::format("{}/{}.ktx", config.dataRootPath + directory.data(), textureNameStr));

		switch (textureType)
		{
			case aiTextureType_DIFFUSE:
				materialManager.GetDiffuseMaterial(mesh.materialId_).SetDiffuse(textureManager.GetImage2d(textureId));
				break;
			case aiTextureType_SPECULAR:
				materialManager.GetDiffuseMaterial(mesh.materialId_).SetSpecular(textureManager.GetImage2d(textureId));
				break;
			case aiTextureType_EMISSIVE:
				break;
			case aiTextureType_HEIGHT:
			case aiTextureType_NORMALS:
				materialManager.GetDiffuseMaterial(mesh.materialId_).SetNormal(textureManager.GetImage2d(textureId));
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
			case _aiTextureType_Force32Bit:
				logDebug("Unsupported texture type");
				break;
		}
	}
}
}
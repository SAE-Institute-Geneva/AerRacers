#include "gl/model_manager.h"

namespace neko::gl
{
ModelManager::ModelManager() { ModelManagerLocator::provide(this); }

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

void ModelManager::SetTexture(ModelId modelId,
    const std::string& texturelPath,
    gl::Mesh::Texture::Type textureType)
{
	if (modelId == INVALID_MODEL_ID) return;

	const auto it = modelMap_.find(modelId);
	if (it == modelMap_.end()) return;
	
    Model model = modelMap_[modelId];
	Mesh* mesh = model.GetMeshPtr(0);
	auto& textures        = mesh->GetTextures();
	const auto typeIndex  = mesh->GetTexture(textureType);
	if (!texturelPath.empty())
	{
		auto& textureManager = TextureManagerLocator::get();
		textures[typeIndex].textureId = textureManager.LoadTexture(texturelPath, Texture::TextureFlags::DEFAULT);
		const auto* texturePtr = textureManager.GetTexture(textures[typeIndex].textureId);
		if (texturePtr)
		{
			textures[typeIndex].textureName = texturePtr->name;
		} else {
			logError("[Error] Texture " + texturelPath + " not loaded");
		}
		textures[typeIndex].type = textureType;
	}
}

std::string ModelManager::GetModelName(ModelId modelId)
{
	for (auto& it : modelPathMap_)
	{
		if (it.second == modelId)
		{
			std::size_t startName = it.first.find_last_of('/') + 1;
			std::size_t endName   = it.first.find_first_of('.');
			std::size_t size      = endName - startName;
			std::string name      = it.first.substr(startName, size);
			return name;
		}
	}

	return "";
}

std::string_view ModelManager::GetModelPath(ModelId modelId)
{
	for (auto& it : modelPathMap_)
		if (it.second == modelId) return it.first;

	return "";
}
}    // namespace neko::gl
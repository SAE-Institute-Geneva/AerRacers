#include "vk/models/model_manager.h"

#include "io_system.h"
#include "engine/engine.h"
#include "vk/commands/model_command_buffer.h"
#include "vk/graphics.h"

namespace neko::vk
{
ModelManager::ModelManager()
{
	importer_.SetIOHandler(new NekoIOSystem());
	ModelManagerLocator::provide(this);
}

void ModelManager::Init() {}

void ModelManager::Update(seconds)
{
	while (!modelLoaders_.empty())
	{
		auto& modelLoader = modelLoaders_.front();
		modelLoader.Update();
		if(modelLoader.HasErrors())
		{
			modelLoaders_.pop();
		}
		else if (modelLoader.IsDone())
		{
			const auto modelId = modelLoader.GetModelId();
			models_[modelId] = *modelLoader.GetModel();
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
	for (auto& model : models_)
	{
		model.second.Destroy();
	}
}

ModelId ModelManager::LoadModel(const std::string& path)
{
	const auto it = modelPathMap_.find(path);
	if (it != modelPathMap_.end())
		return it->second;

	const auto& config = BasicEngine::GetInstance()->config;
	const std::string metaPath = fmt::format("{}{}.meta", config.dataRootPath, path);
	auto metaJson = LoadJson(metaPath);
	ModelId modelId = INVALID_MODEL_ID;
	if (CheckJsonExists(metaJson, "uuid"))
	{
		modelId = sole::rebuild(metaJson["uuid"].get<std::string>());
	}
	else
	{
		logDebug(fmt::format("[Error] Could not find model id in json file: {}", metaPath));
		return modelId;
	}

	modelLoaders_.push(ModelLoader(importer_, path, modelId));
	modelLoaders_.back().Start();
	return modelId;
}

const Model* ModelManager::GetModel(ModelId modelId) const
{
	if (modelId == INVALID_MODEL_ID)
		return nullptr;

	const auto it = models_.find(modelId);
	if (it != models_.end())
		return &it->second;

	return nullptr;
}

bool ModelManager::IsLoaded(ModelId modelId)
{
	return GetModel(modelId) != nullptr;
}
}
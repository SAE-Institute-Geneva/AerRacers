#include "vk/models/model_manager.h"

#include "vk/material/material_manager.h"
#include "vk/models/io_system.h"
#include "vk/vk_resources.h"

namespace neko::vk
{
ModelManager::ModelManager() { ModelManagerLocator::provide(this); }

void ModelManager::Init() {}

void ModelManager::Update(seconds)
{
	while (!modelLoaders_.empty())
	{
		auto& modelLoader = modelLoaders_.front();
		modelLoader.Update();
		if (modelLoader.HasErrors())
		{
			modelLoaders_.pop();
		}
		else if (modelLoader.IsDone())
		{
			const ModelId modelId = modelLoader.GetModelId();
			models_[modelId]      = *modelLoader.GetModel();
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
	VkResources::Inst->GetCurrentCmdBuffer().SubmitIdle(false);

	for (auto& model : models_) model.second.Destroy();
}

ModelId ModelManager::LoadModel(std::string_view path)
{
	const auto it = modelPathMap_.find(path.data());
	if (it != modelPathMap_.end())
	{
		logDebug(fmt::format("[Debug] Model is already loaded: {}", path));
		return it->second;
	}

	logDebug(fmt::format("[Debug] Loading model: {}", path));
	const std::string metaPath  = fmt::format("{}.meta", path);
	const json metaJson         = LoadJson(metaPath);
	ModelId modelId             = INVALID_MODEL_ID;
	if (CheckJsonExists(metaJson, "uuid"))
	{
		modelId = sole::rebuild(metaJson["uuid"].get<std::string>());
	}
	else
	{
		logDebug(fmt::format("[Error] Could not find model id in json file: {}", metaPath));
		return modelId;
	}

	modelPathMap_.emplace(path.data(), modelId);
	modelLoaders_.push(ModelLoader(path, modelId));
	modelLoaders_.back().Start();
	return modelId;
}

const Model* ModelManager::GetModel(ModelId modelId) const
{
	if (modelId == INVALID_MODEL_ID) return nullptr;

	const auto it = models_.find(modelId);
	if (it != models_.end()) return &it->second;

	return nullptr;
}

bool ModelManager::IsLoaded(ModelId modelId)
{
	const auto* model = GetModel(modelId);
	if (!model) return false;

	auto& materialManager = MaterialManagerLocator::get();
	for (const auto& mesh : model->GetMeshes())
		if (!materialManager.IsMaterialLoaded(mesh.GetMaterialId())) return false;

	return true;
}
}    // namespace neko::vk
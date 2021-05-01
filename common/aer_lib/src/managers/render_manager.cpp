#include "aer/managers/render_manager.h"

#include "engine/resource_locations.h"
#include "utils/file_utility.h"

#include "aer/log.h"

#ifdef NEKO_VULKAN
#include "vk/vk_resources.h"
#endif

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::aer
{
RenderManager::RenderManager(EntityManager& entityManager,
#ifdef NEKO_GLES3
	gl::ModelManager& modelManager,
#else
	vk::ModelManager& modelManager,
#endif
	Transform3dManager& transform3DManager,
	LightManager& lightManager)
   : ComponentManager<DrawCmd, EntityMask(ComponentType::MODEL)>(entityManager),
	 modelManager_(modelManager),
	 transformManager_(transform3DManager),
	 lightManager_(lightManager)
{
	DirectionalLight::Instance = &dirLight_;
	instancesMap_.reserve(32);
}

void RenderManager::Init()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("RenderManager::Init", profiler::colors::Brown);
#endif

#ifdef NEKO_GLES3
	preRender_ = Job {[this]()
		{
			shader_.LoadFromFile(
				GetGlShadersFolderPath() + "light.vert", GetGlShadersFolderPath() + "light.frag");
			shader_.BindUbo(gl::kUboMatricesSize, gl::kUboMatricesBinding);
			shader_.BindUbo(gl::kUboLightsSize, gl::kUboLightsBinding);
		}};

	RendererLocator::get().AddPreRenderJob(&preRender_);
#endif
}

void RenderManager::Update(seconds)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("RenderManager::Update", profiler::colors::Brown);
#endif
}

void RenderManager::Render()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("RenderManager::Render", profiler::colors::Brown);
#endif
	const auto& entities =
		entityManager_.get().FilterEntities(static_cast<EntityMask>(ComponentType::MODEL));

#ifdef NEKO_GLES3
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	shader_.Bind();

	const auto& camera = CameraLocator::get();
	shader_.SetVec3("viewPos", camera.position);
	frustum_ = Frustum(camera);
	lightManager_.SetShaderValues(shader_);

	instancesMap_.clear();
	auto& modelManager = gl::ModelManagerLocator::get();
	for (auto& entity : entities)
	{
		if (components_[entity].isVisible && components_[entity].modelId != gl::INVALID_MODEL_ID &&
			modelManager.IsLoaded(components_[entity].modelId))
		{
			Aabb3d aabbModel =
				modelManager.GetModel(components_[entity].modelId)->GetMesh(0).GetAabb();
			int nbMeshes = modelManager.GetModel(components_[entity].modelId)->GetMeshCount();
			for (int i = 1; i < nbMeshes; ++i)
			{
				Aabb3d aabbMesh =
					modelManager.GetModel(components_[entity].modelId)->GetMesh(i).GetAabb();
				aabbModel.lowerLeftBound.x =
					std::min(aabbModel.lowerLeftBound.x, aabbMesh.lowerLeftBound.x);
				aabbModel.lowerLeftBound.y =
					std::min(aabbModel.lowerLeftBound.y, aabbMesh.lowerLeftBound.y);
				aabbModel.lowerLeftBound.z =
					std::min(aabbModel.lowerLeftBound.y, aabbMesh.lowerLeftBound.z);
				aabbModel.upperRightBound.x =
					std::max(aabbModel.upperRightBound.x, aabbMesh.upperRightBound.x);
				aabbModel.upperRightBound.y =
					std::max(aabbModel.upperRightBound.y, aabbMesh.upperRightBound.y);
				aabbModel.upperRightBound.z =
					std::max(aabbModel.upperRightBound.y, aabbMesh.upperRightBound.z);
			}
			aabbModel.FromCenterExtends(
				aabbModel.CalculateCenter() * transformManager_.GetGlobalScale(entity) +
					transformManager_.GetGlobalPosition(entity),
				aabbModel.CalculateExtends() * transformManager_.GetGlobalScale(entity));
			if (frustum_.Contains(aabbModel))
			{
				const Mat4f& modelMat = transformManager_.GetComponent(entity);
				instancesMap_[components_[entity].modelId].push_back(modelMat);
			}
		}
	}

	for (auto&& instance : instancesMap_)
	{
		const auto* model = modelManager.GetModel(instance.first);
		model->DrawInstanced(shader_, instance.second[0], static_cast<int>(instance.second.size()));
	}
#elif NEKO_VULKAN
	lightManager_.SetShaderValues();

	auto& cmdBuffers = vk::VkResources::Inst->modelCommandBuffers;
	for (auto& entity : entities)
	{
		if (!modelManager_.IsLoaded(components_[entity].modelId)) continue;

		const Mat4f& modelMat = transformManager_.GetComponent(entity);
		for (std::size_t i = 0; i < vk::VkResources::Inst->GetViewportCount(); ++i)
			cmdBuffers[i].AddMatrix(components_[entity].modelInstanceIndex, modelMat);
	}
#endif
}

void RenderManager::Destroy()
{
#ifdef NEKO_GLES3
	shader_.Destroy();
#else
	auto& cmdBuffers = vk::VkResources::Inst->modelCommandBuffers;
	for (std::size_t i = 0; i < vk::VkResources::Inst->GetViewportCount(); ++i)
		cmdBuffers[i].Destroy();
#endif
}

std::string RenderManager::GetModelName(Entity entity)
{
	return modelManager_.GetModelName(components_[entity].modelId);
}

std::string_view RenderManager::GetModelPath(Entity entity)
{
	return modelManager_.GetModelPath(components_[entity].modelId);
}

void RenderManager::SetModel(Entity entity, const std::string& modelPath)
{
#ifdef NEKO_GLES3
	gl::ModelId modelId = modelManager_.LoadModel(modelPath);

	SetModel(entity, modelId);
#else
	vk::ModelId modelId = modelManager_.LoadModel(modelPath);

	SetModel(entity, modelId);
#endif
}

#ifdef NEKO_GLES3
void RenderManager::SetModel(Entity entity, gl::ModelId modelId)
{
	DrawCmd drawCmd = GetComponent(entity);
	drawCmd.modelId = modelId;

	SetComponent(entity, drawCmd);
}

void RenderManager::SetDiffuseTexture(Entity entity,
	const TextureName& diffuseTexture)
{
	DrawCmd drawCmd = GetComponent(entity);
	drawCmd.diffuseTexture = diffuseTexture;

	SetComponent(entity, drawCmd);
}
#else
void RenderManager::DestroyComponent(Entity entity)
{
	ComponentManager::DestroyComponent(entity);

	components_[entity] = DrawCmd();
}

void RenderManager::SetModel(Entity entity, vk::ModelId modelId)
{
	components_[entity].modelId = modelId;

	auto& cmdBuffers = vk::VkResources::Inst->modelCommandBuffers;
	const Mat4f& modelMat = transformManager_.GetComponent(entity);
	if (components_[entity].modelInstanceIndex == INVALID_INDEX)
	{
		for (std::size_t i = 0; i < vk::VkResources::Inst->GetViewportCount(); ++i)
			components_[entity].modelInstanceIndex =
				cmdBuffers[i].AddModelInstanceIndex(components_[entity].modelId, modelMat);
	}
	else
	{
		for (std::size_t i = 0; i < vk::VkResources::Inst->GetViewportCount(); ++i)
			cmdBuffers[i].SetModelId(
				components_[entity].modelInstanceIndex, components_[entity].modelId);
	}
}
#endif

void RenderManager::UpdateDirtyComponent(Entity entity)
{
	ComponentManager::UpdateDirtyComponent(entity);
}

RendererViewer::RendererViewer(EntityManager& entityManager, RenderManager& rendererManager)
   : ComponentViewer(entityManager), rendererManager_(rendererManager)
{}

void RendererViewer::DrawImGui(Entity entity)
{
	if (entity == INVALID_ENTITY) return;
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::MODEL)))
	{
		if (ImGui::TreeNode("Renderer"))
		{
			std::string meshName = "ModelName: " + rendererManager_.GetModelName(entity);
			ImGui::Text("%s", meshName.c_str());

			if (ImGui::Button("Change Model"))
			{
				ImGui::SetNextWindowSize(Vec2f(300.0f, 0.0f));
				ImGui::OpenPopup("Test");
			}

			if (ImGui::BeginPopupModal("Test"))
			{
				ImGui::InputText("New Model Name", &newNameBuffer_, ImGui::LabelPos::LEFT);
				if (ImGui::Button("Set Model"))
				{
					std::transform(newNameBuffer_.begin(),
						newNameBuffer_.end(),
						newNameBuffer_.begin(),
						[](unsigned char c) { return std::tolower(c); });
					std::size_t found    = newNameBuffer_.find_last_of('/') + 1;
					std::string meshName = newNameBuffer_;
					if (found != std::string::npos) meshName = meshName.substr(found);

					const std::string path =
						GetModelsFolderPath() + newNameBuffer_ + "/" + meshName + ".obj";
					if (FileExists(path))
					{
						rendererManager_.SetModel(entity, path);
						ImGui::CloseCurrentPopup();
					}
				}

				if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

				ImGui::EndPopup();
			}

			ImGui::TreePop();
		}
	}
}

json RendererViewer::GetJsonFromComponent(Entity entity) const
{
	json rendererComponent = json::object();
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::MODEL)))
		if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
			rendererComponent["meshName"] = rendererManager_.GetModelName(entity);

	return rendererComponent;
}

void RendererViewer::SetComponentFromJson(Entity entity, const json& componentJson)
{
	if (CheckJsonParameter(componentJson, "meshName", json::value_t::string))
	{
		std::string meshPath = std::string(componentJson["meshName"]);
		std::transform(meshPath.begin(),
			meshPath.end(),
			meshPath.begin(),
			[](unsigned char c) { return std::tolower(c); });
		std::size_t found = meshPath.find_last_of('/') + 1;
		std::string meshName = meshPath;
		if (found != std::string::npos)
			meshName = meshName.substr(found);

		const std::string path = GetModelsFolderPath() + meshPath + "/" + meshName + ".obj";
		if (FileExists(path))
		{
			rendererManager_.AddComponent(entity);
			rendererManager_.SetModel(entity, path);
		}
		else
		{
			LogDebug("File " + meshPath + " not found");
		}
	}
}
}    // namespace neko::aer

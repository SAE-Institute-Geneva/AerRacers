#include "aer/managers/render_manager.h"

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
}

void RenderManager::Init()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("RenderManager::Init", profiler::colors::Brown);
#endif
#ifdef NEKO_GLES3
	const auto& config = BasicEngine::GetInstance()->GetConfig();

	preRender_ = Job {[this, config]()
		{
			shader_.LoadFromFile(config.dataRootPath + "shaders/opengl/light.vert",
				config.dataRootPath + "shaders/opengl/light.frag");
			shader_.BindUbo(gl::kUboMatricesSize, gl::kUboMatricesBinding);
			shader_.BindUbo(gl::kUboLightsSize, gl::kUboLightsBinding);
		}};

	RendererLocator::get().AddPreRenderJob(&preRender_);
#else
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
	lightManager_.SetShaderValues(shader_);

	auto& modelManager = gl::ModelManagerLocator::get();
	for (auto& entity : entities)
	{
		if (components_[entity].isVisible && components_[entity].modelId != gl::INVALID_MODEL_ID &&
			modelManager.IsLoaded(components_[entity].modelId))
		{
			const Mat4f& modelMat = transformManager_.GetComponent(entity);
			shader_.SetMat4("model", modelMat);
			shader_.SetMat3("normalMatrix", Mat3f(modelMat)/*.Inverse().Transpose()*/);

			const auto& model = modelManager.GetModel(components_[entity].modelId);
			model->Draw(shader_);
		}
	}
#elif NEKO_VULKAN
	auto& modelCommandBuffers = vk::VkResources::Inst->modelCommandBuffers;
	for (auto& entity : entities)
	{
		if (!modelManager_.IsLoaded(components_[entity].modelId)) continue;

		const Mat4f& modelMat = transformManager_.GetComponent(entity);
		for (auto& modelCommandBuffer : modelCommandBuffers)
			modelCommandBuffer.AddMatrix(components_[entity].modelInstanceIndex, modelMat);
	}
#endif
}

void RenderManager::Destroy()
{
#ifdef NEKO_GLES3
	shader_.Destroy();
#else
	for (auto& modelCommandBuffer : vk::VkResources::Inst->modelCommandBuffers)
		modelCommandBuffer.Destroy();
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
#else
void RenderManager::DestroyComponent(Entity entity)
{
	ComponentManager::DestroyComponent(entity);

	components_[entity] = DrawCmd();
}

void RenderManager::SetModel(Entity entity, vk::ModelId modelId)
{
	components_[entity].modelId = modelId;

	const Mat4f& modelMat = transformManager_.GetComponent(entity);
	if (components_[entity].modelInstanceIndex == INVALID_INDEX)
	{
		for (auto& modelCommandBuffer : vk::VkResources::Inst->modelCommandBuffers)
			components_[entity].modelInstanceIndex =
				modelCommandBuffer.AddModelInstanceIndex(components_[entity].modelId, modelMat);
	}
	else
	{
		for (auto& modelCommandBuffer : vk::VkResources::Inst->modelCommandBuffers)
			modelCommandBuffer.SetModelId(
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
		Configuration config = BasicEngine::GetInstance()->GetConfig();
		std::string meshName = std::string(componentJson["meshName"]);
		std::transform(meshName.begin(),
			meshName.end(),
			meshName.begin(),
			[](unsigned char c) { return std::tolower(c); });

		const std::string path =
			config.dataRootPath + "models/" + meshName + "/" + meshName + ".obj";
		if (FileExists(path))
		{
			rendererManager_.AddComponent(entity);
			rendererManager_.SetModel(entity, path);
		}
		else
		{
			LogDebug("File " + meshName + " not found");
		}
	}
}
}    // namespace neko::aer
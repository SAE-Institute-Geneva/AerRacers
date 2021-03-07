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
	RendererViewer& rendererViewer)
#ifdef NEKO_GLES3
   : ComponentManager<DrawCmd, EntityMask(ComponentType::MODEL)>(entityManager),
#else
   : ComponentManager<DrawCmd, EntityMask(ComponentType::MODEL)>(entityManager),
#endif
	 modelManager_(modelManager),
	 transformManager_(transform3DManager),
	 dirtyManager_(entityManager),
	 rendererViewer_(rendererViewer)
{
	entityManager_.get().RegisterOnChangeParent(this);
	dirtyManager_.RegisterComponentManager(this);
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
			shader_.BindUbo(2 * sizeof (Mat4f));
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
	dirtyManager_.UpdateDirtyEntities();
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

	//TODO find a way to cleanly implement lighting
	/*shader_.SetUInt("lightType", lightType_);

	shader_.SetVec3("light.position", pointLight_.position);
	shader_.SetVec3("light.direction", directionalLight_.direction.Normalized());
	shader_.SetVec3("light.ambient", pointLight_.ambient);
	shader_.SetVec3("light.diffuse", pointLight_.diffuse);
	shader_.SetFloat("light.specular", pointLight_.specular);
	shader_.SetFloat("light.intensity", pointLight_.intensity);

	shader_.SetFloat("sLight.blend", Cos(spotLight_.angle * spotLight_.blend));
	shader_.SetFloat("sLight.angle", Cos(spotLight_.angle));
	shader_.SetFloat("sLight.radius", spotLight_.radius);*/

	auto& modelManager = gl::ModelManagerLocator::get();
	for (auto& entity : entities)
	{
		if (components_[entity].isVisible && components_[entity].modelId != gl::INVALID_MODEL_ID &&
			modelManager.IsLoaded(components_[entity].modelId))
		{
			const Mat4f& modelMat = transformManager_.GetComponent(entity);
			shader_.SetMat4("model", modelMat);
			shader_.SetMat3("normalMatrix", Mat3f(modelMat).Inverse().Transpose());

			const auto& model = modelManager.GetModel(components_[entity].modelId);
			model->Draw(shader_);
		}
	}
#elif NEKO_VULKAN
	vk::VkResources* vkObj = vk::VkResources::Inst;
	for (auto& entity : entities)
	{
		if (!modelManager_.IsLoaded(components_[entity].modelId)) continue;

		const Mat4f& modelMat = transformManager_.GetComponent(entity);
		vkObj->modelCommandBuffer.AddMatrix(components_[entity].modelInstanceIndex, modelMat);
	}
#endif
}

void RenderManager::Destroy()
{
#ifdef NEKO_GLES3
	shader_.Destroy();
#else
	vk::VkResources::Inst->modelCommandBuffer.Destroy();
#endif
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
    rendererViewer_.SetMeshName(entity, modelPath);
	dirtyManager_.SetDirty(entity);
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
		components_[entity].modelInstanceIndex =
			vk::VkResources::Inst->modelCommandBuffer.AddModelInstanceIndex(
				components_[entity].modelId, modelMat);
	}
	else
	{
		vk::VkResources::Inst->modelCommandBuffer.SetModelId(
			components_[entity].modelInstanceIndex, components_[entity].modelId);
	}
}
#endif

void RenderManager::UpdateDirtyComponent(Entity entity)
{
	ComponentManager::UpdateDirtyComponent(entity);
}

void RenderManager::OnChangeParent(Entity, Entity, Entity) {}

RendererViewer::RendererViewer(EntityManager& entityManager, RenderManager& renderManager)
   : ComponentViewer(entityManager), rendererManager_(renderManager)
{
	ResizeIfNecessary(meshNames_, INIT_ENTITY_NMB - 1, std::string());
}

void RendererViewer::DrawImGui(Entity entity)
{
	if (entity == INVALID_ENTITY) return;
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::MODEL)))
	{
		if (ImGui::TreeNode("Renderer"))
		{
			ResizeIfNecessary(meshNames_, entity, std::string());
			std::string meshName = "MeshName : " + meshNames_[entity];
			ImGui::Text("%s", meshName.c_str());
			ImGui::TreePop();
		}
	}
}

void RendererViewer::SetMeshName(Entity entity, std::string_view meshName)
{
	if (entity == INVALID_ENTITY) return;

	ResizeIfNecessary(meshNames_, entity, std::string());
	std::size_t startName = meshName.find_last_of('/') + 1;
	std::size_t endName   = meshName.find_first_of('.');
	std::size_t size      = endName - startName;
	std::string_view name = meshName.substr(startName, size);
	meshNames_[entity]    = name;
}

std::string_view RendererViewer::GetMeshName(Entity entity) const
{
	if (entity == INVALID_ENTITY && entity >= meshNames_.size()) return "";
	return meshNames_[entity];
}

json RendererViewer::GetJsonFromComponent(Entity entity) const
{
	json rendererComponent = json::object();
	if (entityManager_.HasComponent(entity, EntityMask(ComponentType::MODEL)))
	{
		if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
		{
			Configuration config          = BasicEngine::GetInstance()->GetConfig();
			rendererComponent["meshName"] = meshNames_[entity];
		}
	}
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
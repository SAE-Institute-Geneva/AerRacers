#include "aer/managers/render_manager.h"

#include <imgui.h>

#ifdef EASY_PROFILE_USE
    #include "easy/profiler.h"
#endif

#include "engine/engine.h"
#include "graphics/camera.h"
#include <aer/log.h>
#include <utils/file_utility.h>

namespace neko::aer
{
RenderManager::RenderManager(
	EntityManager& entityManager,
#ifdef NEKO_GLES3
	gl::ModelManager& modelManager,
#endif
	Transform3dManager& transform3DManager,
    RendererViewer& rendererViewer)
#ifdef NEKO_GLES3
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
	const auto& config = BasicEngine::GetInstance()->GetConfig();

	preRender_ = Job {[this, config]()
		{
			shader_.LoadFromFile(config.dataRootPath + "shaders/opengl/light.vert",
				config.dataRootPath + "shaders/opengl/light.frag");
			shader_.BindUbo(2 * sizeof (Mat4f));
		}};

	RendererLocator::get().AddPreRenderJob(&preRender_);
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
		const auto& drawCmd = GetComponent(entity);
		if (drawCmd.isVisible &&
			drawCmd.modelId != gl::INVALID_MODEL_ID &&
			modelManager.IsLoaded(drawCmd.modelId))
		{
			const Mat4f& modelMat = transformManager_.GetComponent(entity);
			shader_.SetMat4("model", modelMat);
			shader_.SetMat3("normalMatrix", Mat3f(modelMat).Inverse().Transpose());

			const auto& model = modelManager.GetModel(drawCmd.modelId);
			model->Draw(shader_);
		}
	}
#endif
}

void RenderManager::Destroy()
{
	shader_.Destroy();
}

void RenderManager::SetModel(Entity entity, const std::string& modelPath)
{
#ifdef NEKO_GLES3
	auto& modelManager  = gl::ModelManagerLocator::get();
	gl::ModelId modelId = modelManager.LoadModel(modelPath);

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
#endif

void RenderManager::UpdateDirtyComponent(Entity entity)
{
	ComponentManager::UpdateDirtyComponent(entity);
}

void RenderManager::OnChangeParent(Entity, Entity, Entity) {}

RendererViewer::RendererViewer(EntityManager& entityManager, RenderManager& renderManager) : ComponentViewer(entityManager), rendererManager_(renderManager)
{
    ResizeIfNecessary(meshNames_, INIT_ENTITY_NMB - 1, std::string());
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
    if(CheckJsonParameter(componentJson, "meshName", json::value_t::string))
    {
        Configuration config       = BasicEngine::GetInstance()->GetConfig();
        std::string meshName = std::string(componentJson["meshName"]);
        std::transform(meshName.begin(), meshName.end(), meshName.begin(),
                       [](unsigned char c){ return std::tolower(c); });

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

void RendererViewer::DrawImGui(Entity entity)
{
    if (entity == INVALID_ENTITY) return;
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::MODEL))) {
        if (ImGui::TreeNode("Renderer")) {
            ResizeIfNecessary(meshNames_, entity, std::string());
            std::string meshName = "MeshName : " + meshNames_[entity];
            ImGui::Text("%s", meshName.c_str());
            ImGui::TreePop();
        }
    }
}

void neko::aer::RendererViewer::SetMeshName(Entity entity, const std::string& meshPath)
{
    if (entity == INVALID_ENTITY) return;
    ResizeIfNecessary(meshNames_, entity, std::string());
    auto startName       = meshPath.find_last_of('/')+1;
    auto endName         = meshPath.find_first_of('.');
    auto size            = endName - startName;
    std::string meshName = meshPath.substr(startName, size);
    meshNames_[entity] = meshName;
}

std::string RendererViewer::GetMeshName(Entity entity) const
{
    if (entity == INVALID_ENTITY && entity >= meshNames_.size()) return "";
    return meshNames_[entity];
}
}    // namespace neko::aer
#include "aer/managers/render_manager.h"

#include "engine/engine.h"
#include "graphics/camera.h"
#include <aer\log.h>
#include <utils/file_utility.h>

namespace neko::aer
{
RenderManager::RenderManager(
	EntityManager& entityManager,
#ifdef NEKO_GLES3
	gl::ModelManager& modelManager,
#endif
	Transform3dManager& transform3DManager)
#ifdef NEKO_GLES3
   : ComponentManager<DrawCmd, EntityMask(ComponentType::MODEL)>(entityManager),
#endif
	 modelManager_(modelManager),
	 transformManager_(transform3DManager),
	 dirtyManager_(entityManager)
{
	entityManager_.get().RegisterOnChangeParent(this);
	dirtyManager_.RegisterComponentManager(this);
}

void RenderManager::Init()
{
	const auto& config = BasicEngine::GetInstance()->GetConfig();

	preRender_ = Job {[this, config]()
		{
			shader_.LoadFromFile(config.dataRootPath + "shaders/opengl/light.vert",
				config.dataRootPath + "shaders/opengl/light.frag");
		}};

	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void RenderManager::Update(seconds)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Update Model");
#endif
	dirtyManager_.UpdateDirtyEntities();

	RendererLocator::get().Render(this);
}

void RenderManager::Render()
{
	const auto& entities =
		entityManager_.get().FilterEntities(static_cast<EntityMask>(ComponentType::MODEL));

#ifdef NEKO_GLES3
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	shader_.Bind();

	const auto& camera = CameraLocator::get();
	const Mat4f camView = camera.GenerateViewMatrix();
	const Mat4f camProj = camera.GenerateProjectionMatrix();

	shader_.SetMat4("view", camView);
	shader_.SetMat4("proj", camProj);

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
json RenderManager::GetJsonFromComponent(Entity entity) const
{
	//TODO(@Luca) RenderViewer
    return json();
}

void RenderManager::SetComponentFromJson(Entity entity, const json& componentJson)
{
    if (CheckJsonParameter(componentJson, "meshName", json::value_t::string))
    {
        Configuration config       = BasicEngine::GetInstance()->GetConfig();
        const std::string meshName = std::string(componentJson["meshName"]);
        const std::string path =
            config.dataRootPath + "models/" + meshName + "/" + meshName + ".obj";
        if (FileExists(path))
        {
            AddComponent(entity);
            SetModel(entity, path);
        }
        else
        {
            LogDebug("File " + meshName + " not found");
        }
    }
}

void RenderManager::DrawImGui(Entity)
{
    
}
}    // namespace neko::aer
#pragma once
#include "engine/transform.h"

#ifdef NEKO_GLES3
#include "gl/model.h"
#include "gl/model_manager.h"
#else
#include "vk/commands/model_command_buffer.h"
#endif

#include "aer/frustum.h"
#include "aer/managers/light_manager.h"

namespace neko::aer
{
struct DrawCmd
{
#ifdef NEKO_GLES3
	gl::ModelId modelId = gl::INVALID_MODEL_ID;
#else
	vk::ModelId modelId = vk::INVALID_MODEL_ID;
	vk::ModelInstanceIndex modelInstanceIndex = INVALID_INDEX;
#endif

	bool isVisible = true;
	TextureName diffuseTexture = INVALID_TEXTURE_NAME;
};

class RenderManager final : public SystemInterface,
							public RenderCommandInterface,
							public ComponentManager<DrawCmd, EntityMask(ComponentType::MODEL)>
{
public:
	RenderManager(EntityManager& entityManager,
#ifdef NEKO_GLES3
		gl::ModelManager& modelManager,
#else
		vk::ModelManager& modelManager,
#endif
		Transform3dManager& transform3DManager,
		LightManager& lightManager);

	void Init() override;
	void Update(seconds) override;
	void Render() override;
	void Destroy() override;

	[[nodiscard]] std::string GetModelName(Entity entity);
	[[nodiscard]] std::string_view GetModelPath(Entity entity);

#ifdef NEKO_GLES3
	gl::Shader& GetShader() { return shader_; }

	void SetModel(Entity entity, gl::ModelId modelId);
    void SetDiffuseTexture(Entity entity, const TextureName& diffuseTexture);
#else
	void DestroyComponent(Entity entity) override;

	void SetModel(Entity entity, vk::ModelId modelId);
#endif
	void SetModel(Entity entity, const std::string& modelPath);

	void UpdateDirtyComponent(Entity entity) override;

protected:
#ifdef NEKO_GLES3
	gl::Shader shader_;
	gl::ModelManager& modelManager_;
#else
	vk::ModelManager& modelManager_;
#endif

	Frustum frustum_;

	Transform3dManager& transformManager_;
	LightManager& lightManager_;

	DirectionalLight dirLight_ {};

	std::unordered_map<gl::ModelId, std::vector<Mat4f>> instancesMap_;

	Job preRender_;
};

/**
 * \brief The Component Manager use to serialize to json and imgui components
 */
class RendererViewer final : public ComponentViewer
{
public:
	RendererViewer(EntityManager& entityManager, RenderManager& rendererManager);
	~RendererViewer() override = default;

	/**
     * \brief Draw the Imgui with the component parameter of an entity
     */
	void DrawImGui(Entity entity) override;

	/**
	 * \brief Set a component of an entity from a json of the component
	 * \componentJson json object with component parameter
	 */
	void SetComponentFromJson(Entity entity, const json& componentJson) override;

	/**
	 * \brief Get a json object of the component of an entity
	 * \return json object with component parameter
	 */
	[[nodiscard]] json GetJsonFromComponent(Entity entity) const override;

private:
	RenderManager& rendererManager_;
};
}

#pragma once
#include "engine/transform.h"

#ifdef NEKO_GLES3
#include "gl/model.h"
#else
#include "vk/commands/model_command_buffer.h"
#endif

namespace neko::aer
{
class RendererViewer;

#ifdef NEKO_GLES3
struct DrawCmd
{
	gl::ModelId modelId = gl::INVALID_MODEL_ID;

	bool isVisible = true;
};
#else
struct DrawCmd
{
	vk::ModelId modelId = vk::INVALID_MODEL_ID;
	vk::ModelInstanceIndex modelInstanceIndex = INVALID_INDEX;

	bool isVisible = true;
};
#endif

class IRenderManager : public OnChangeParentInterface
{
public:
	virtual void UpdateDirtyComponent(Entity entity) = 0;
};

class RenderManager : public IRenderManager,
					  public SystemInterface,
					  public RenderCommandInterface,
#ifdef NEKO_GLES3
					  public ComponentManager<DrawCmd, EntityMask(ComponentType::MODEL)>
#else
					  public ComponentManager<DrawCmd, EntityMask(ComponentType::MODEL)>
#endif
{
public:
	explicit RenderManager(EntityManager& entityManager,
#ifdef NEKO_GLES3
		gl::ModelManager& modelManager,
#else
		vk::ModelManager& modelManager,
#endif
		Transform3dManager& transform3DManager,
		RendererViewer& rendererViewer);

	void Init() override;
	void Update(seconds) override;
	void Render() override;
	void Destroy() override;

	void SetModel(Entity entity, const std::string& modelPath);
#ifdef NEKO_GLES3
	void SetModel(Entity entity, gl::ModelId modelId);
#else
	void DestroyComponent(Entity entity) override;

	void SetModel(Entity entity, vk::ModelId modelId);
#endif

	void UpdateDirtyComponent(Entity entity) override;
	void OnChangeParent(Entity entity, Entity newParent, Entity oldParent) override;

protected:
#ifdef NEKO_GLES3
	gl::Shader shader_;
	gl::ModelManager& modelManager_;
#else
	vk::ModelManager& modelManager_;
#endif

	Transform3dManager& transformManager_;

	DirtyManager dirtyManager_;

	RendererViewer& rendererViewer_;

	Job preRender_;
};

/**
 * \brief The Component Manager use to serialize to json and imgui components
 */
class RendererViewer final : public ComponentViewer
{
public:
	explicit RendererViewer(EntityManager& entityManager, RenderManager& renderManager);
	~RendererViewer() override = default;

	/**
     * \brief Draw the Imgui with the component parameter of an entity
     */
	void DrawImGui(Entity entity) override;

	/**
	 * \brief Return the mesh name of a model
	 */
	[[nodiscard]] std::string_view GetMeshName(Entity entity) const;

	/**
     * \brief Use to store the meshName 
     * \param meshName meshName of the model
     */
	void SetMeshName(Entity entity, std::string_view meshName);

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

	/**
     * \brief Vector of meshName only use for serialization
     */
	std::vector<std::string> meshNames_;
};
}

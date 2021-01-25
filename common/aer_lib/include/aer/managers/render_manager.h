#pragma once
#include "engine/component.h"
#include "engine/transform.h"
#include "gl/model.h"
#include "mathematics/matrix.h"

namespace neko::aer
{
class RendererViewer;

#ifdef NEKO_GLES3
struct DrawCmd
{
	gl::ModelId modelId = gl::INVALID_MODEL_ID;

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
#endif
{
public:
	explicit RenderManager(
		EntityManager& entityManager,
#ifdef NEKO_GLES3
		gl::ModelManager& modelManager,
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
#endif

	void UpdateDirtyComponent(Entity entity) override;
	void OnChangeParent(Entity entity, Entity newParent, Entity oldParent) override;

protected:
#ifdef NEKO_GLES3
	gl::Shader shader_;
	gl::ModelManager& modelManager_;
#endif

	Transform3dManager& transformManager_;

	DirtyManager dirtyManager_;

	RendererViewer& rendererViewer_;

	Job preRender_;
};

/**
 * \brief The Component Manager use to serialize to json and imgui components
 */
class RendererViewer : public ComponentViewer
{
public:
    explicit RendererViewer(EntityManager& entityManager, RenderManager& renderManager);

    virtual ~RendererViewer() = default;

    /**
     * \brief Get a json object of the component of an entity
     * \return json object with component parameter
     */
    json GetJsonFromComponent(Entity entity) const override;

    /**
     * \brief Set a component of an entity from a json of the component
     * \componentJson json object with component parameter
     */
    void SetComponentFromJson(Entity entity, const json& componentJson) override;

    /**
     * \brief Draw the Imgui with the component parameter of an entity
     */
    void DrawImGui(Entity entity) override;

    /**
     * \brief Use to store the meshName 
     * \param meshName meshName of the model
     */
    void SetMeshName(Entity entity, const std::string& meshName);
    /**
     * \brief Return the mesh name of a model
     */
    std::string GetMeshName(Entity entity) const;

private:
    RenderManager& rendererManager_;

    /**
     * \brief Vector of meshName only use for serialization
     */
    std::vector<std::string> meshNames_;
};
}

#pragma once
#include "engine/component.h"
#include "engine/transform.h"
#include "gl/model.h"
#include "mathematics/matrix.h"

namespace neko::aer
{
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
		Transform3dManager& transform3DManager);

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

    virtual json GetJsonFromComponent(Entity) const override;
    virtual void SetComponentFromJson(Entity, const json&) override;
    virtual void DrawImGui(Entity) override;
protected:
#ifdef NEKO_GLES3
	gl::Shader shader_;
	gl::ModelManager& modelManager_;
#endif

	Transform3dManager& transformManager_;

	DirtyManager dirtyManager_;

	Job preRender_;
};
}

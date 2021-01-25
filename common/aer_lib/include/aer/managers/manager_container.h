#pragma once
#ifdef NEKO_GLES3
#include "gl/model.h"
#endif

#include "aer/managers/render_manager.h"
#include "engine/transform.h"

namespace neko::aer
{
struct ResourceManagerContainer : public SystemInterface
{
	void Init() override
	{
		textureManager.Init();
		modelManager.Init();
	}

	void Update(seconds dt) override
	{
		textureManager.Update(dt);
		modelManager.Update(dt);
	}

	void Destroy() override
	{
		textureManager.Destroy();
		modelManager.Destroy();
	}

#ifdef NEKO_GLES3
	gl::TextureManager textureManager;
	gl::ModelManager modelManager;
#endif
};

struct ComponentManagerContainer : public SystemInterface
{
	ComponentManagerContainer(ResourceManagerContainer& rContainer)
	   : transform3dManager(entityManager),
		 renderManager(entityManager, rContainer.modelManager, transform3dManager, rendererViewer),
		 sceneManager(entityManager, *this),
		 rendererViewer(entityManager, renderManager),
		 transform3dViewer(entityManager, transform3dManager)
	{}

	void Init() override
	{
		transform3dManager.Init();
		renderManager.Init();
	}

	void Update(seconds dt) override
	{
		transform3dManager.Update();
		renderManager.Update(dt);
	}

	void Destroy() override { renderManager.Destroy(); }

	EntityManager entityManager;
	Transform3dManager transform3dManager;
	RenderManager renderManager;

	Transform3dViewer transform3dViewer;
    RendererViewer rendererViewer;

	SceneManager sceneManager;
};
}

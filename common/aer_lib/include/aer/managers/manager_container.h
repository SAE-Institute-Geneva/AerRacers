#pragma once
#ifdef NEKO_GLES3
#include "gl/model.h"
#endif

#include "aer/aer_engine.h"
#include "aer/managers/render_manager.h"
#include "engine/transform.h"

namespace neko::aer
{
struct ResourceManagerContainer
{
#ifdef NEKO_GLES3
	gl::TextureManager textureManager;
	gl::ModelManager modelManager;
#endif
};

struct ComponentManagerContainer
{
	ComponentManagerContainer(ResourceManagerContainer& rContainer)
		: transform3dManager(entityManager),
		  renderManager(entityManager, rContainer.modelManager, transform3dManager),
		  sceneManager(entityManager, transform3dManager)
	{}

	EntityManager entityManager;
	Transform3dManager transform3dManager;
	RenderManager renderManager;

	SceneManager sceneManager;
};
}

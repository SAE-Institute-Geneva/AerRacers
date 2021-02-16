#pragma once
#ifdef NEKO_GLES3
#include "gl/model.h"
#endif

#include "aer/managers/render_manager.h"
#include "engine/transform.h"
#include "px/rigidbody.h"
#include "px/physics_engine.h"
#include "aer/managers/ship_controller_manager.h"
#include "aer/managers/camera_controller_manager.h"

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
	ComponentManagerContainer(ResourceManagerContainer& rContainer, physics::PhysicsEngine& physicsEngine)
		: transform3dManager(entityManager),
		renderManager(entityManager, rContainer.modelManager, transform3dManager, rendererViewer),
		rigidDynamicManager(entityManager, transform3dManager, physicsEngine),
		rigidStaticManager(entityManager, transform3dManager, physicsEngine),
		shipControllerManager(entityManager, transform3dManager, rigidDynamicManager, rigidStaticManager, physicsEngine),
		cameraControllerManager(entityManager, transform3dManager, rigidDynamicManager, physicsEngine),
		transform3dViewer(entityManager, transform3dManager),
		rendererViewer(entityManager, renderManager),
		rigidDynamicViewer(transform3dManager, entityManager, physicsEngine, rigidDynamicManager),
		rigidStaticViewer(transform3dManager, entityManager, physicsEngine, rigidStaticManager),
		shipControllerViewer(entityManager, shipControllerManager),
		cameraControllerViewer(entityManager, cameraControllerManager),
		sceneManager(entityManager, *this)
	{
        physicsEngine.RegisterFixedUpdateListener(rigidDynamicManager);
        physicsEngine.RegisterFixedUpdateListener(rigidStaticManager);
        physicsEngine.RegisterFixedUpdateListener(rigidStaticViewer);
        physicsEngine.RegisterFixedUpdateListener(rigidDynamicViewer);
	}

	void Init() override
	{
		transform3dManager.Init();
		renderManager.Init();
		shipControllerManager.Init();
	}

	void Update(seconds dt) override
	{
		transform3dManager.Update();
		renderManager.Update(dt);
		shipControllerManager.Update(dt);
	}

	void Destroy() override { renderManager.Destroy(); }

	EntityManager entityManager;
	Transform3dManager transform3dManager;
    RenderManager renderManager;
    physics::RigidDynamicManager rigidDynamicManager;
    physics::RigidStaticManager rigidStaticManager;
	ShipControllerManager shipControllerManager;
	CameraControllerManager cameraControllerManager;

	Transform3dViewer transform3dViewer;
    RendererViewer rendererViewer;
    physics::RigidDynamicViewer rigidDynamicViewer;
    physics::RigidStaticViewer rigidStaticViewer;
	ShipControllerViewer shipControllerViewer;
	CameraControllerViewer cameraControllerViewer;
	SceneManager sceneManager;
};
}

#pragma once
#include "px/physics_engine.h"

#ifdef NEKO_VULKAN
#include "vk/material/material_manager.h"
#endif

#include "aer/managers/render_manager.h"
#include "aer/scene.h"

#ifdef NEKO_FMOD
#include "fmod/audio_manager.h"
#endif

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
#else
	vk::TextureManager textureManager;
	vk::ModelManager modelManager;
	vk::MaterialManager materialManager;
#endif
};

struct ComponentManagerContainer : public SystemInterface
{
	ComponentManagerContainer(
		ResourceManagerContainer& rContainer, physics::PhysicsEngine& physicsEngine)
	   : transform3dManager(entityManager),
		 renderManager(entityManager, rContainer.modelManager, transform3dManager, lightManager),
		 lightManager(entityManager, transform3dManager),
		 rigidDynamicManager(entityManager, transform3dManager, physicsEngine),
		 rigidStaticManager(entityManager, transform3dManager, renderManager, physicsEngine),
#ifdef NEKO_FMOD
		 audioManager(entityManager, transform3dManager),
#endif
		 transform3dViewer(entityManager, transform3dManager),
		 rendererViewer(entityManager, renderManager),
		 lightViewer(entityManager, lightManager),
		 rigidDynamicViewer(transform3dManager, entityManager, physicsEngine, rigidDynamicManager),
		 rigidStaticViewer(transform3dManager, entityManager, physicsEngine, rigidStaticManager),
#ifdef NEKO_FMOD
		audioViewer(entityManager, audioManager),
#endif
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
#ifdef NEKO_FMOD
		audioManager.Init();
#endif
	}

	void Update(seconds dt) override
	{
		transform3dManager.Update();
		renderManager.Update(dt);
#ifdef NEKO_FMOD
		audioManager.Update(dt);
#endif
	}

	void Destroy() override
	{
		renderManager.Destroy();
#ifdef NEKO_FMOD
		audioManager.Destroy();
#endif
	}

	EntityManager entityManager;
	Transform3dManager transform3dManager;
	RenderManager renderManager;
	LightManager lightManager;
	physics::RigidDynamicManager rigidDynamicManager;
	physics::RigidStaticManager rigidStaticManager;
#ifdef NEKO_FMOD
	AudioManager audioManager;
#endif

	Transform3dViewer transform3dViewer;
	RendererViewer rendererViewer;
	LightViewer lightViewer;
	physics::RigidDynamicViewer rigidDynamicViewer;
	physics::RigidStaticViewer rigidStaticViewer;
#ifdef NEKO_FMOD
	AudioViewer audioViewer;
#endif

	SceneManager sceneManager;
};
}    // namespace neko::aer

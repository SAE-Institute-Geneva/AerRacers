#pragma once
#include "px/physics_engine.h"
#include "px/rigidbody.h"
#include "px/physics_engine.h"
#ifdef NEKO_GLES3
#include "gl/texture.h"
#include "gl/model.h"
#elif NEKO_VULKAN
#include "vk/material/material_manager.h"
#endif

#include "aer/managers/light_manager.h"
#include "aer/managers/player_manager.h"
#include "aer/managers/render_manager.h"
#include "aer/managers/ship_controller_manager.h"
#include "aer/managers/camera_controller_manager.h"
#include "aer/managers/waypoint_manager.h"
#include "aer/managers/game_manager.h"
#include "aer/managers/menu_manager.h"

#include "aer/scene.h"
#include "engine/transform.h"

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
	ComponentManagerContainer(AerEngine& engine,
		ResourceManagerContainer& rContainer, physics::PhysicsEngine& physicsEngine)
	   : transform3dManager(entityManager),
		 renderManager(entityManager, rContainer.modelManager, transform3dManager, lightManager),
		 lightManager(entityManager, transform3dManager),
		 rigidDynamicManager(entityManager, transform3dManager, physicsEngine),
    rigidStaticManager(entityManager, transform3dManager, renderManager, physicsEngine),
#ifdef NEKO_FMOD
		audioManager(entityManager, transform3dManager),
#endif
         playerManager(*this, engine),
         shipInputManager(playerManager),
         shipControllerManager(  
            physicsEngine,
             *this),
         cameraControllerManager(
            entityManager,
            transform3dManager,
            rigidDynamicManager,
            physicsEngine,
            playerManager, 
            engine),
		 transform3dViewer(entityManager, transform3dManager),
		 rendererViewer(entityManager, renderManager),
		 lightViewer(entityManager, lightManager),
		 rigidDynamicViewer(transform3dManager, entityManager, physicsEngine, rigidDynamicManager),
		 rigidStaticViewer(transform3dManager, entityManager, physicsEngine, rigidStaticManager),
         shipControllerViewer(entityManager, playerManager, shipControllerManager),
         cameraControllerViewer(entityManager, playerManager, cameraControllerManager),
    waypointManager(engine), gameManager(engine), menuManager(engine),
#ifdef NEKO_FMOD
		audioViewer(entityManager, audioManager),
#endif
		 sceneManager(entityManager, *this)
	{
        physicsEngine.RegisterCollisionListener(shipControllerManager);
        physicsEngine.RegisterFixedUpdateListener(rigidDynamicManager);
        physicsEngine.RegisterFixedUpdateListener(rigidStaticManager);
        physicsEngine.RegisterFixedUpdateListener(rigidStaticViewer);
        physicsEngine.RegisterFixedUpdateListener(rigidDynamicViewer);
        physicsEngine.RegisterFixedUpdateListener(shipControllerManager);
        physicsEngine.RegisterFixedUpdateListener(cameraControllerManager);
	}
	
    void Init() override
    {
        menuManager.Init();
        gameManager.Init();
        transform3dManager.Init();
        renderManager.Init();
#ifdef NEKO_FMOD
		audioManager.Init();
#endif
        playerManager.Init();
        shipControllerManager.Init();
        waypointManager.Init();
    }

    void Update(seconds dt) override
    {
        transform3dManager.Update();
        renderManager.Update(dt);
#ifdef NEKO_FMOD
		audioManager.Update(dt);
#endif
        playerManager.Update(dt);
        shipControllerManager.Update(dt);
        cameraControllerManager.Update(dt);
        waypointManager.Update(dt);
        shipInputManager.Update(dt);
        gameManager.Update(dt);
        menuManager.Update(dt);
        transform3dManager.Update();
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
    PlayerManager playerManager;
    ShipInputManager shipInputManager;
    ShipControllerManager shipControllerManager;
    CameraControllerManager cameraControllerManager;
    WaypointManager waypointManager;
    GameManager gameManager;
    MenuManager menuManager;

    Transform3dViewer transform3dViewer;
    RendererViewer rendererViewer;
    LightViewer lightViewer;
    physics::RigidDynamicViewer rigidDynamicViewer;
    physics::RigidStaticViewer rigidStaticViewer;
    ShipControllerViewer shipControllerViewer;
    CameraControllerViewer cameraControllerViewer;
    SceneManager sceneManager;
#ifdef NEKO_FMOD
	AudioManager audioManager;
	AudioViewer audioViewer;
#endif
};
}    // namespace neko::aer


#include "aer/managers/player_manager.h"
#include "aer/managers/manager_container.h"
#include "engine/engine.h"

namespace neko::aer
{
	int PlayerManager::CreatePlayer(Vec3f pos)
	{
        const auto& config = BasicEngine::GetInstance()->GetConfig();
        cContainer_.sceneManager.AddTag("Ship");
        cContainer_.sceneManager.AddTag("Camera");
        Entity shipEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.entityManager.SetEntityName(shipEntity, "ship");
        TagLocator::get().SetEntityTag(shipEntity, "Ship");
        cContainer_.transform3dManager.AddComponent(shipEntity);
        cContainer_.transform3dManager.SetGlobalPosition(shipEntity, pos);
        physics::RigidDynamicData rigidDynamic;
        rigidDynamic.useGravity = false;
        rigidDynamic.colliderType = physics::ColliderType::BOX;
        rigidDynamic.angularDamping = 3.0f;
        rigidDynamic.freezeRotation = Vec3<bool>(true, false, true);
        rigidDynamic.boxColliderData.size = Vec3f(9.0f, 1.0f, 5.5f);
        rigidDynamic.material.bouciness = 0.0f;
        rigidDynamic.material.staticFriction = 0.0f;
        rigidDynamic.material.dynamicFriction = 0.0f;
        cContainer_.rigidDynamicManager.AddRigidDynamic(shipEntity, rigidDynamic);
        cContainer_.shipControllerManager.AddComponent(shipEntity);
        Entity shipModelEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipModelEntity);
        cContainer_.transform3dManager.SetGlobalPosition(shipModelEntity, pos);
        cContainer_.transform3dManager.SetGlobalScale(shipModelEntity, Vec3f(9.0f, 1.0f, 5.5f));
        cContainer_.entityManager.SetEntityParent(shipModelEntity, shipEntity);
        cContainer_.renderManager.AddComponent(shipModelEntity);
        cContainer_.renderManager.SetModel(shipModelEntity, config.dataRootPath + "models/cube/cube.obj");

        Entity cameraEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.entityManager.SetEntityName(cameraEntity, "cameraEntity");
        TagLocator::get().SetEntityTag(shipEntity, "Camera");
        cContainer_.transform3dManager.AddComponent(cameraEntity);
        cContainer_.cameraControllerManager.AddComponent(cameraEntity);
        PlayerComponent playerComponent;
        playerComponent.cameraEntity = cameraEntity;
        playerComponent.shipEntity = shipEntity;
        playerComponent.shipEntity = shipModelEntity;
        playerComponent.playerNumber = playerComponents_.size();
        playerComponents_.push_back(playerComponent);

		return playerComponent.playerNumber;
	}
}


#include "aer/managers/player_manager.h"
#include "aer/managers/manager_container.h"
#include "engine/engine.h"
#include <aer\log.h>

namespace neko::aer
{
    PlayerManager::PlayerManager(ComponentManagerContainer& cContainer)
        : cContainer_(cContainer),
        cameraControllerManager_(cContainer.cameraControllerManager),
        shipControllerManager_(cContainer.shipControllerManager),
        shipInputManager_(cContainer.shipInputManager)
    {
        playerComponents_.resize(INIT_PLAYER_NMB, PlayerComponent());
    }

    PlayerId PlayerManager::CreatePlayer(Vec3f pos, EulerAngles euler)
    {
        if (playerCount_ >= INIT_PLAYER_NMB) {
            LogError("Max Player Create");
            return playerCount_ - 1;
        }
        const auto& config = BasicEngine::GetInstance()->GetConfig();
        cContainer_.sceneManager.AddTag("Ship");
        cContainer_.sceneManager.AddTag("Camera");
        Entity shipEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.entityManager.AddComponentType(shipEntity, EntityMask(ComponentType::PLAYER_COMPONENT));
        cContainer_.entityManager.SetEntityName(shipEntity, "ship");
        TagLocator::get().SetEntityTag(shipEntity, "Ship");
        cContainer_.transform3dManager.AddComponent(shipEntity);
        cContainer_.transform3dManager.SetGlobalPosition(shipEntity, pos);
        cContainer_.transform3dManager.SetGlobalRotation(shipEntity, euler);
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
        shipControllerManager_.InitComponent(playerCount_);
        Entity shipModelEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(shipModelEntity);
        cContainer_.transform3dManager.SetGlobalPosition(shipModelEntity, pos);
        cContainer_.transform3dManager.SetGlobalScale(shipModelEntity, Vec3f(9.0f, 1.0f, 5.5f));
        cContainer_.entityManager.SetEntityParent(shipModelEntity, shipEntity);
        cContainer_.renderManager.AddComponent(shipModelEntity);
        cContainer_.renderManager.SetModel(shipModelEntity, config.dataRootPath + "models/cube/cube.obj");

        PlayerComponent playerComponent;
        playerComponent.shipEntity = shipEntity;
        playerComponent.shipModelEntity = shipModelEntity;
        playerComponent.playerNumber = playerComponents_.size();
        playerComponent.playerSpawn = pos;
        std::vector<sdl::ControllerId> controllers = sdl::InputLocator::get().GetControllerIdVector();
        if (controllers.size() > playerCount_)
            playerComponent.linkedJoystick = controllers[playerCount_];
        playerComponents_[playerCount_] = playerComponent;
        playerCount_++;

        return playerComponent.playerNumber;
    }

    PlayerComponent PlayerManager::GetPlayerComponent(PlayerId playerId)
    {
        return playerComponents_[playerId];
    }

    Entity PlayerManager::GetShipEntity(PlayerId playerId)
    {
        return playerComponents_[playerId].shipEntity;
    }

    void PlayerManager::Init()
    {

    }

    void PlayerManager::Update(seconds dt)
    {
        for (PlayerId playerId = 0; playerId < GetPlayerCount(); ++playerId) {
            std::vector<sdl::ControllerId> controllers = sdl::InputLocator::get().GetControllerIdVector();
            if (controllers.size() > playerId)
            {
                playerComponents_[playerId].linkedJoystick = controllers[playerId];
            }
            else if (!controllers.empty()) {
                playerComponents_[playerId].linkedJoystick = controllers[0];
            }
            else {
                playerComponents_[playerId].linkedJoystick = 0;
            }
            if (cContainer_.transform3dManager.GetGlobalPosition(playerComponents_[playerId].shipEntity).y < yDespawnPosition)
            {
                cContainer_.rigidDynamicManager.MovePosition(playerComponents_[playerId].shipEntity, playerComponents_[playerId].playerSpawn);
            }
        }
    }

    void PlayerManager::Destroy()
    {

    }
}
